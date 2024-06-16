#pragma once

#include "stringtype.h"
#include "arraylist.h"
#include "file.h"

typedef enum {
    //keywords
    TOK_FALSE,
    TOK_NONE,
    TOK_TRUE,
    TOK_AND,
    TOK_AS,
    TOK_ASSERT,
    TOK_BREAK,
    TOK_CLASS,
    TOK_CONTINUE,
    TOK_DEF,
    TOK_DEL,
    TOK_ELIF,
    TOK_ELSE,
    TOK_EXCEPT,
    TOK_FINALLY,
    TOK_FOR,
    TOK_FROM,
    TOK_GLOBAL,
    TOK_IF,
    TOK_IMPORT,
    TOK_IN,
    TOK_IS,
    TOK_LAMBDA,
    TOK_NONLOCAL,
    TOK_NOT,
    TOK_OR,
    TOK_PASS,
    TOK_RAISE,
    TOK_RETURN,
    TOK_TRY,
    TOK_WHILE,
    TOK_WITH,
    TOK_YIELD,

    //operators 
    TOK_ADD,
    TOK_SUB,
    TOK_MUL,
    TOK_DIV,
    TOK_MOD,
    TOK_EXP,
    TOK_FLOOR_DIV,
    TOK_BIT_AND,
    TOK_BIT_OR,
    TOK_BIT_XOR,
    TOK_BIT_NOT,
    TOK_LEFT_SHIFT,
    TOK_RIGHT_SHIFT,


    //bool operators 
    TOK_LESS_THAN,
    TOK_GREATER_THAN,
    TOK_LESS_EQUAL,
    TOK_GREATER_EQUAL,
    TOK_EQUAL,
    TOK_NOT_EQUAL,
    TOK_ASSIGN,

    //assignment operators
    TOK_ADD_ASSIGN,
    TOK_SUB_ASSIGN,
    TOK_MUL_ASSIGN,
    TOK_DIV_ASSIGN,
    TOK_MOD_ASSIGN,
    TOK_EXP_ASSIGN,
    TOK_FLOOR_DIV_ASSIGN,
    TOK_LEFT_SHIFT_ASSIGN,
    TOK_RIGHT_SHIFT_ASSIGN,
    TOK_BIT_AND_ASSIGN,
    TOK_BIT_OR_ASSIGN,
    TOK_BIT_XOR_ASSIGN,



    //containers
    TOK_LEFT_PAREN,
    TOK_RIGHT_PAREN,
    TOK_LEFT_BRACE,
    TOK_RIGHT_BRACE,
    TOK_LEFT_BRACKET,
    TOK_RIGHT_BRACKET,
    TOK_SQUOTE,
    TOK_DQUOTE,

    TOK_EOF,
    TOK_TAB, 
    TOK_NEW_LINE, 
    TOK_COMMA,
    TOK_COLON,
    TOK_SEMICOLON,
    TOK_DOT,
    TOK_ELLIPSIS,
    TOK_AT,


    TOK_IDENTIFIER,
    TOK_INTEGER,
    TOK_FLOAT,
    TOK_STRING,

    TOK_UNKOWN,
    TOK_MAX, //used as a dummy value  
} TokenType;


typedef struct {
    TokenType type;
    String literal;
    unsigned int line;
} Token;


ArrayList tokenize_file(File* file);

void token_delete(Token* t);
