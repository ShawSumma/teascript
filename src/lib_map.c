/*
** lib_map.c
** Teascript Map class
*/

#define lib_map_c
#define TEA_CORE

#include "tealib.h"

#include "tea_vm.h"
#include "tea_map.h"
#include "tea_list.h"
#include "tea_lib.h"

static void map_len(tea_State* T)
{
    if(tea_get_top(T) != 1) tea_error(T, "readonly property");
    tea_push_number(T, tea_len(T, 0));
}

static void map_keys(tea_State* T)
{
    if(tea_get_top(T) != 1) tea_error(T, "readonly property");
    GCmap* map = mapV(T->base);

    tea_new_list(T);

    GClist* list = listV(T->base + 1);
    for(int i = 0; i < map->size; i++)
    {
        if(map->entries[i].empty) continue;
        tea_list_add(T, list, &map->entries[i].key);
    }
}

static void map_values(tea_State* T)
{
    if(tea_get_top(T) != 1) tea_error(T, "readonly property");
    GCmap* map = mapV(T->base);

    tea_new_list(T);

    GClist* list = listV(T->base + 1);
    for(int i = 0; i < map->size; i++)
    {
        if(map->entries[i].empty) continue;
        tea_list_add(T, list, &map->entries[i].value);
    }
}

static void map_init(tea_State* T)
{
    tea_new_map(T);
}

static void map_get(tea_State* T)
{
    int count = tea_get_top(T);
    tea_check_args(T, count < 2 || count > 3, "Expected 1 or 2 arguments, got %d", count);
    tea_opt_any(T, 2);
    GCmap* map = tea_lib_checkmap(T, 0);
    TValue* key = T->base + 1;
    cTValue* o = tea_map_get(map, key);
    if(o)
    {
        copyTV(T, T->top++, o);
    }
}

static void map_set(tea_State* T)
{
    int count = tea_get_top(T);
    tea_check_args(T, count < 2 || count > 3, "Expected 1 or 2 arguments, got %d", count);
    tea_opt_any(T, 2);
    GCmap* map = tea_lib_checkmap(T, 0);
    TValue* key = tea_lib_checkany(T, 1);
    TValue* value = T->base + 2;
    cTValue* o = tea_map_get(map, key);
    if(!o)
    {
        copyTV(T, tea_map_set(T, map, key), value);
    }
    else
    {
        copyTV(T, T->base + 2, o);
    }
}

static void map_update(tea_State* T)
{
    GCmap* map = tea_lib_checkmap(T, 0);
    GCmap* m = tea_lib_checkmap(T, 1);
    tea_map_merge(T, m, map);
    tea_push_value(T, 0);
}

static void map_clear(tea_State* T)
{
    GCmap* map = tea_lib_checkmap(T, 0);
    tea_map_clear(T, map);
}

static void map_contains(tea_State* T)
{
    GCmap* map = tea_lib_checkmap(T, 0);
    TValue* o = tea_lib_checkany(T, 1);
    tea_push_bool(T, tea_map_get(map, o) != NULL);
}

static void map_delete(tea_State* T)
{
    GCmap* map = tea_lib_checkmap(T, 0);
    TValue* key = tea_lib_checkany(T, 1);
    if(!tea_map_delete(T, map, key))
    {
        tea_err_msg(T, TEA_ERR_MAPKEY);
    }
    tea_push_value(T, 0);
}

static void map_copy(tea_State* T)
{
    GCmap* map = tea_lib_checkmap(T, 0);
    GCmap* newmap = tea_map_copy(T, map);
    setmapV(T, T->top++, newmap);
}

static void map_foreach(tea_State* T)
{
    GCmap* map = tea_lib_checkmap(T, 0);
    tea_check_function(T, 1);

    for(int i = 0; i < map->size; i++)
    {
        if(map->entries[i].empty) continue;

        TValue* key = &map->entries[i].key;
        TValue* value = &map->entries[i].value;

        tea_push_value(T, 1);
        copyTV(T, T->top++, key);
        copyTV(T, T->top++, value);
        tea_call(T, 2);
        tea_pop(T, 1);
    }
    tea_set_top(T, 1);
}

static void map_iterate(tea_State* T)
{
    GCmap* map = tea_lib_checkmap(T, 0);
    if(map->count == 0)
    {
        tea_push_nil(T);
        return;
    }

    /* If we're starting the iteration, start at the first used entry */
    int index = 0;

    /* Otherwise, start one past the last entry we stopped at */
    if(!tea_is_nil(T, 1))
    {
        if(!tea_is_number(T, 1))
        {
            tea_error(T, "Expected a number to iterate");
        }

        index = (uint32_t)tea_get_number(T, 1);
        if(index < 0)
        {
            tea_push_nil(T);
            return;
        }

        if(index >= map->size)
        {
            tea_push_nil(T);
            return;
        }

        /* Advance the iterator */
        index++;
    }

    /* Find a used entry, if any */
    for(; index < map->size; index++)
    {
        if(!map->entries[index].empty)
        {
            tea_push_number(T, index);
            return;
        }
    }

    /* If we get here, walked all of the entries */
    tea_push_nil(T);
}

static void map_iteratorvalue(tea_State* T)
{
    GCmap* map = tea_lib_checkmap(T, 0);
    int index = tea_check_number(T, 1);
    MapEntry* entry = &map->entries[index];
    if(entry->empty)
    {
        tea_error(T, "Invalid map iterator");
    }
    tea_new_list(T);
    copyTV(T, T->top++, &entry->key);
    tea_add_item(T, 2);
    copyTV(T, T->top++, &entry->value);
    tea_add_item(T, 2);
}

static void map_opadd(tea_State* T)
{
    GCmap* m1 = tea_lib_checkmap(T, 0);
    GCmap* m2 = tea_lib_checkmap(T, 1);
    GCmap* map = tea_map_new(T);
    setmapV(T, T->top++, map);
    tea_map_merge(T, m1, map);
    tea_map_merge(T, m2, map);
    tea_pop(T, 3);
    setmapV(T, T->top++, map);
}

/* ------------------------------------------------------------------------ */

static const tea_Class map_class[] = {
    { "len", "property", map_len, TEA_VARARGS },
    { "keys", "property", map_keys, TEA_VARARGS },
    { "values", "property", map_values, TEA_VARARGS },
    { "init", "method", map_init, 1 },
    { "get", "method", map_get, TEA_VARARGS },
    { "set", "method", map_set, TEA_VARARGS },
    { "update", "method", map_update, 2 },
    { "clear", "method", map_clear, 1 },
    { "contains", "method", map_contains, 2 },
    { "delete", "method", map_delete, 2 },
    { "copy", "method", map_copy, 1 },
    { "foreach", "method", map_foreach, 2 },
    { "iterate", "method", map_iterate, 2 },
    { "iteratorvalue", "method", map_iteratorvalue, 2 },
    { "+", "static", map_opadd, 2 },
    { NULL, NULL, NULL }
};

void tea_open_map(tea_State* T)
{
    tea_create_class(T, TEA_CLASS_MAP, map_class);
    T->map_class = classV(T->top - 1);
    tea_set_global(T, TEA_CLASS_MAP);
    tea_push_nil(T);
}