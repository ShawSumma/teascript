/*
** tea_token.h
** Teascript tokens
*/

#ifndef TEA_TOKEN_H
#define TEA_TOKEN_H

#include "tea_def.h"
#include "tea_value.h"

typedef enum
{
    // Single-character tokens
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_DOT,
    TOKEN_COLON, TOKEN_QUESTION,
    TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SLASH, TOKEN_STAR,

    // One or two character tokens
    TOKEN_PLUS_PLUS, TOKEN_MINUS_MINUS,
    TOKEN_PLUS_EQUAL, TOKEN_MINUS_EQUAL, TOKEN_STAR_EQUAL, TOKEN_SLASH_EQUAL,
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,
    TOKEN_PERCENT, TOKEN_PERCENT_EQUAL,
    TOKEN_STAR_STAR, TOKEN_STAR_STAR_EQUAL,
    TOKEN_DOT_DOT, TOKEN_DOT_DOT_DOT,

    TOKEN_AMPERSAND, TOKEN_AMPERSAND_EQUAL,
    TOKEN_PIPE, TOKEN_PIPE_EQUAL,
    TOKEN_CARET, TOKEN_CARET_EQUAL,
    TOKEN_TILDE,
    TOKEN_ARROW,
    TOKEN_GREATER_GREATER,
    TOKEN_LESS_LESS,

    // Literals
    TOKEN_NAME, TOKEN_STRING, TOKEN_INTERPOLATION, TOKEN_NUMBER,

    // Keywords
    TOKEN_AND, TOKEN_CLASS, TOKEN_STATIC, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUNCTION, TOKEN_CASE, TOKEN_SWITCH, TOKEN_DEFAULT,
    TOKEN_IF, TOKEN_NULL, TOKEN_OR, TOKEN_IS,
    TOKEN_IMPORT, TOKEN_FROM, TOKEN_AS, TOKEN_ENUM,
    TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_CONTINUE, TOKEN_BREAK, TOKEN_IN,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_CONST,
    TOKEN_WHILE, TOKEN_DO,

    TOKEN_ERROR, TOKEN_EOF
} TeaTokenType;

typedef struct
{
    TeaTokenType type;
    const char* start;
    int length;
    int line;
    TeaValue value;
} TeaToken;

#endif