/*
** tea_err.c
** Error handling
*/

#define tea_err_c
#define TEA_CORE

#include <setjmp.h>
#include <stdlib.h>

#include "tea_def.h"
#include "tea_err.h"
#include "tea_vm.h"
#include "tea_func.h"

struct tea_longjmp
{
    struct tea_longjmp* prev;
    jmp_buf buf;
    volatile int status;
};

/* Error message strings */
TEA_DATADEF const char* tea_err_allmsg =
#define ERRDEF(name, msg) msg "\0"
#include "tea_errmsg.h"
;

/* Runtime error */
TEA_NOINLINE void tea_err_run(tea_State* T, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);

    for(CallInfo* ci = T->ci; ci > T->ci_base; ci--)
    {
        /* Skip stack trace for C functions */
        if(ci->func == NULL) continue;

        GCproto* proto = ci->func->proto;
        size_t instruction = ci->ip - proto->code - 1;
        fprintf(stderr, "[line %d] in ", tea_func_getline(proto, instruction));
        fprintf(stderr, "%s\n", proto->name->chars);
    }

    tea_err_throw(T, TEA_ERROR_RUNTIME);
}

/* Lexer error */
TEA_NOINLINE void tea_err_lex(tea_State* T, const char* src, const char* tok, int line, const char* message)
{
    fprintf(stderr, "File %s, [line %d] Error", src, line);

    if(tok != NULL)
    {
        fprintf(stderr, " at '%s'", tok);
    }

    fprintf(stderr, ": %s\n", message);

    tea_err_throw(T, TEA_ERROR_SYNTAX);
}

/* Throw error */
TEA_NOINLINE void tea_err_throw(tea_State* T, int code)
{
    if(T->error_jump)
    {
        T->error_jump->status = code;
        TEA_THROW(T);
    }
    else
    {
        T->panic(T);
        exit(EXIT_FAILURE);
    }
}

int tea_err_protected(tea_State* T, tea_CPFunction f, void* ud)
{
    struct tea_longjmp tj;
    tj.status = TEA_OK;
    tj.prev = T->error_jump;    /* Chain new error handler */
    T->error_jump = &tj;
    TEA_TRY(T, &tj,
        (*f)(T, ud);
    );
    T->error_jump = tj.prev;    /* Restore old error handler */
    return tj.status;
}

TEA_API tea_CFunction tea_atpanic(tea_State* T, tea_CFunction panicf)
{
    tea_CFunction old = T->panic;
    T->panic = panicf;
    return old;
}

TEA_API void tea_error(tea_State* T, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char msg[1024];
    int len = vsnprintf(NULL, 0, fmt, args);
    vsnprintf(msg, len + 1, fmt, args);
    va_end(args);

    tea_err_run(T, msg);
}