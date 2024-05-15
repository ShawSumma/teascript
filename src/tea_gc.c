/*
** tea_gc.c
** Garbage collector
*/

#include <stdlib.h>

#define tea_gc_c
#define TEA_CORE

#include "tea_state.h"
#include "tea_gc.h"
#include "tea_parse.h"
#include "tea_tab.h"
#include "tea_func.h"
#include "tea_udata.h"

#ifdef TEA_DEBUG_LOG_GC
#include <stdio.h>
#include "tea_debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2

/* -- Collector -------------------------------------------------- */

static void gc_blacken(tea_State* T, GCobj* obj)
{
    switch(obj->gct)
    {
        case TEA_TMODULE:
        {
            GCmodule* module = (GCmodule*)obj;
            tea_gc_markobj(T, (GCobj*)module->name);
            tea_gc_markobj(T, (GCobj*)module->path);
            tea_tab_mark(T, &module->values);
            break;
        }
        case TEA_TLIST:
        {
            GClist* list = (GClist*)obj;
            for(int i = 0; i < list->count; i++)
            {
                tea_gc_markval(T, list_slot(list, i));
            }
            break;
        }
        case TEA_TMAP:
        {
            GCmap* map = (GCmap*)obj;
            for(int i = 0; i < map->size; i++)
            {
                MapEntry* item = &map->entries[i];
                tea_gc_markval(T, &item->key);
                tea_gc_markval(T, &item->value);
            }
            break;
        }
        case TEA_TMETHOD:
        {
            GCmethod* bound = (GCmethod*)obj;
            tea_gc_markval(T, &bound->receiver);
            tea_gc_markobj(T, (GCobj*)bound->method);
            break;
        }
        case TEA_TCLASS:
        {
            GCclass* klass = (GCclass*)obj;
            tea_gc_markobj(T, (GCobj*)klass->name);
            tea_gc_markobj(T, (GCobj*)klass->super);
            tea_tab_mark(T, &klass->methods);
            break;
        }
        case TEA_TFUNC:
        {
            GCfunc* func = (GCfunc*)obj;
            if(isteafunc(func))
            {
                tea_gc_markobj(T, (GCobj*)func->t.proto);
                for(int i = 0; i < func->t.upvalue_count; i++)
                {
                    tea_gc_markobj(T, (GCobj*)func->t.upvalues[i]);
                }
            }
            else
            {
                for(int i = 0; i < func->c.upvalue_count; i++)
                {
                    tea_gc_markval(T, &func->c.upvalues[i]);
                }
            }
            break;
        }
        case TEA_TPROTO:
        {
            GCproto* proto = (GCproto*)obj;
            tea_gc_markobj(T, (GCobj*)proto->name);
            for(int i = 0; i < proto->k_count; i++)
            {
                tea_gc_markval(T, proto_kgc(proto, i));
            }
            break;
        }
        case TEA_TINSTANCE:
        {
            GCinstance* instance = (GCinstance*)obj;
            tea_gc_markobj(T, (GCobj*)instance->klass);
            tea_tab_mark(T, &instance->fields);
            break;
        }
        case TEA_TUPVALUE:
        {
            GCupvalue* uv = (GCupvalue*)obj;
            tea_gc_markval(T, &uv->closed);
            break;
        }
        case TEA_TSTRING:
        case TEA_TRANGE:
            break;
    }
}

static void gc_free(tea_State* T, GCobj* obj)
{
    switch(obj->gct)
    {
        case TEA_TRANGE:
        {
            tea_mem_freet(T, GCrange, obj);
            break;
        }
        case TEA_TMODULE:
        {
            GCmodule* module = (GCmodule*)obj;
            tea_tab_free(T, &module->values);
            tea_mem_freet(T, GCmodule, obj);
            break;
        }
        case TEA_TLIST:
        {
            GClist* list = (GClist*)obj;
            tea_mem_freevec(T, TValue, list->items, list->size);
            tea_mem_freet(T, GClist, obj);
            break;
        }
        case TEA_TMAP:
        {
            GCmap* map = (GCmap*)obj;
            tea_mem_freevec(T, MapEntry, map->entries, map->size);
            tea_mem_freet(T, GCmap, obj);
            break;
        }
        case TEA_TMETHOD:
        {
            tea_mem_freet(T, GCmethod, obj);
            break;
        }
        case TEA_TCLASS:
        {
            GCclass* klass = (GCclass*)obj;
            tea_tab_free(T, &klass->methods);
            tea_mem_freet(T, GCclass, obj);
            break;
        }
        case TEA_TFUNC:
        {
            GCfunc* func = (GCfunc*)obj;
            tea_func_free(T, func);
            break;
        }
        case TEA_TPROTO:
        {
            GCproto* proto = (GCproto*)obj;
            tea_mem_freevec(T, BCIns, proto->bc, proto->bc_size);
            tea_mem_freevec(T, LineStart, proto->lines, proto->line_size);
            tea_mem_freevec(T, TValue, proto->k, proto->k_size);
            tea_mem_freet(T, GCproto, obj);
            break;
        }
        case TEA_TINSTANCE:
        {
            GCinstance* instance = (GCinstance*)obj;
            tea_tab_free(T, &instance->fields);
            tea_mem_freet(T, GCinstance, obj);
            break;
        }
        case TEA_TSTRING:
        {
            GCstr* str = (GCstr*)obj;
            tea_str_free(T, str);
            break;
        }
        case TEA_TUSERDATA:
        {
            GCudata* ud = (GCudata*)obj;
            tea_udata_free(T, ud);
            break;
        }
        case TEA_TUPVALUE:
        {
            tea_mem_freet(T, GCupvalue, obj);
            break;
        }
    }
}

/* Mark GC roots */
static void gc_mark_roots(tea_State* T)
{
    for(TValue* slot = T->stack; slot < T->top; slot++)
    {
        tea_gc_markval(T, slot);
    }

    for(CallInfo* ci = T->ci_base; ci <= T->ci; ci++)
    {
        tea_gc_markobj(T, (GCobj*)ci->func);
    }

    for(GCupvalue* upvalue = T->open_upvalues; upvalue != NULL; upvalue = upvalue->next)
    {
        tea_gc_markobj(T, (GCobj*)upvalue);
    }

    tea_tab_mark(T, &T->modules);
    tea_tab_mark(T, &T->globals);

    tea_gc_markobj(T, (GCobj*)T->number_class);
    tea_gc_markobj(T, (GCobj*)T->bool_class);
    tea_gc_markobj(T, (GCobj*)T->func_class);
    tea_gc_markobj(T, (GCobj*)T->list_class);
    tea_gc_markobj(T, (GCobj*)T->map_class);
    tea_gc_markobj(T, (GCobj*)T->string_class);
    tea_gc_markobj(T, (GCobj*)T->range_class);

    if(T->parser != NULL)
    {
        tea_parse_mark(T, T->parser);
    }
}

static void gc_trace_references(tea_State* T)
{
    while(T->gc.gray_count > 0)
    {
        GCobj* obj = T->gc.gray_stack[--T->gc.gray_count];
        gc_blacken(T, obj);
    }
}

static void gc_sweep(tea_State* T)
{
    GCobj* prev = NULL;
    GCobj* obj = T->gc.objects;

    while(obj != NULL)
    {
        if(obj->marked)
        {
            if(obj->marked != TEA_GC_FIXED)
                obj->marked = false;
            prev = obj;
            obj = obj->next;
        }
        else
        {
            GCobj* unreached = obj;
            obj = obj->next;
            if(prev != NULL)
            {
                prev->next = obj;
            }
            else
            {
                T->gc.objects = obj;
            }
            gc_free(T, unreached);
        }
    }
}

/* Mark a TValue (if needed) */
void tea_gc_markval(tea_State* T, TValue* o)
{
    if(tvisgcv(o))
        tea_gc_markobj(T, gcV(o));
}

/* Mark a GC object (if needed) */
void tea_gc_markobj(tea_State* T, GCobj* obj)
{
    if(obj == NULL)
        return;
    if(obj->marked)
        return;

    obj->marked = true;

    if(T->gc.gray_size < T->gc.gray_count + 1)
    {
        T->gc.gray_size = TEA_MEM_GROW(T->gc.gray_size);
        T->gc.gray_stack = (GCobj**)T->allocf(T->allocd, T->gc.gray_stack, 0, sizeof(GCobj*) * T->gc.gray_size);

        if(T->gc.gray_stack == NULL)
        {
            puts(str_data(T->memerr));
            exit(1);
        }
    }

    T->gc.gray_stack[T->gc.gray_count++] = obj;
}

/* Perform a GC collection */
void tea_gc_collect(tea_State* T)
{
#ifdef TEA_DEBUG_LOG_GC
    printf("-- gc begin\n");
    size_t before = T->gc.total;
#endif

    gc_mark_roots(T);
    gc_trace_references(T);
    tea_tab_white(&T->strings);
    gc_sweep(T);
    tea_buf_shrink(T, &T->tmpbuf);  /* Shrink temp buffer */
    tea_buf_shrink(T, &T->strbuf);  /* Shrink buffer */

    T->gc.next_gc = T->gc.total * GC_HEAP_GROW_FACTOR;

#ifdef TEA_DEBUG_LOG_GC
    printf("-- gc end\n");
    printf("   collected %llu bytes (from %llu to %llu) next at %llu\n", before - T->gc.total, before, T->gc.total, T->next_gc);
#endif
}

/* Free all remaining GC objects */
void tea_gc_freeall(tea_State* T)
{
    GCobj* obj = T->gc.objects;
    while(obj != NULL)
    {
        GCobj* next = obj->next;
        gc_free(T, obj);
        obj = next;
    }

    /* Free the gray stack */
    T->allocf(T->allocd, T->gc.gray_stack, sizeof(GCobj*) * T->gc.gray_size, 0);
}

/* -- Allocator -------------------------------------------------- */

/* Call pluggable memory allocator to allocate or resize a fragment */
void* tea_mem_realloc(tea_State* T, void* pointer, size_t old_size, size_t new_size)
{
    tea_assertT((old_size == 0) == (pointer == NULL), "realloc API violation");
    T->gc.total += new_size - old_size;

    if(new_size > old_size)
    {
#ifdef TEA_DEBUG_STRESS_GC
        tea_gc_collect(T);
#endif

        if(T->gc.total > T->gc.next_gc)
        {
            tea_gc_collect(T);
        }
    }

    pointer = T->allocf(T->allocd, pointer, old_size, new_size);
    if(pointer == NULL && new_size > 0)
    {
        puts(str_data(T->memerr));
        exit(1);
    }
    tea_assertT((new_size == 0) == (pointer == NULL), "allocf API violation");
    return pointer;
}

/* Allocate new GC object and link it to the objects root */
GCobj* tea_mem_newgco(tea_State* T, size_t size, uint8_t type)
{
    GCobj* obj = (GCobj*)tea_mem_realloc(T, NULL, 0, size);
    obj->gct = type;
    obj->marked = false;
    obj->next = T->gc.objects;
    T->gc.objects = obj;
    return obj;
}

/* Resize growable vector */
void* tea_mem_grow(tea_State* T, void* pointer, uint32_t* size, size_t size_elem, int limit)
{
    size_t new_size = (*size) << 1;
    if(new_size < TEA_MIN_VECSIZE)
        new_size = TEA_MIN_VECSIZE;
    if(new_size > limit)
        new_size = limit;
    pointer = tea_mem_realloc(T, pointer, (*size) * size_elem, new_size * size_elem);
    *size = new_size;
    return pointer;
}