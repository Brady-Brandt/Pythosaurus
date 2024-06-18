#pragma once
#include "tokenizer.h"
#include "arraylist.h"
#include <stdbool.h>
#include <setjmp.h>
#include <stdnoreturn.h>
#include "file.h"

/*
 *  mod = Module(stmt* body)
        | Interactive(stmt* body)
        | Expression(expr body)

        | Suite(stmt* body)

              
             -- the following expression can appear in assignment context
         | Attribute(expr value, identifier attr, expr_context ctx)
         | Subscript(expr value, slice slice, expr_context ctx)
         | Starred(expr value, expr_context ctx)
         | Name(identifier id, expr_context ctx)
         | List(expr* elts, expr_context ctx)
         | Tuple(expr* elts, expr_context ctx)

          -- col_offset is the byte offset in the utf8 string the parser uses
          attributes (int lineno, int col_offset)

    expr_context = Load | Store | Del | AugLoad | AugStore | Param

    slice = Slice(expr? lower, expr? upper, expr? step)
          | ExtSlice(slice* dims)
          | Index(expr value)

    boolop = And | Or

    operator = Add | Sub | Mult | MatMult | Div | Mod | Pow | LShift
                 | RShift | BitOr | BitXor | BitAnd | FloorDiv

    unaryop = Invert | Not | UAdd | USub

    cmpop = Eq | NotEq | Lt | LtE | Gt | GtE | Is | IsNot | In | NotIn

    comprehension = (expr target, expr iter, expr* ifs)

    excepthandler = ExceptHandler(expr? type, identifier? name, stmt* body)
                    attributes (int lineno, int col_offset)

    arguments = (arg* args, arg? vararg, arg* kwonlyargs, expr* kw_defaults,
                 arg? kwarg, expr* defaults)

    arg = (identifier arg, expr? annotation)
           attributes (int lineno, int col_offset)

    -- keyword arguments supplied to call (NULL identifier for **kwargs)
    keyword = (identifier? arg, expr value)

    -- import name with optional 'as' alias.
    alias = (identifier name, identifier? asname)

    withitem = (expr context_expr, expr? optional_vars)
 *
 *
 *
*/

#define PARSER_ERROR 10 

typedef struct {
    bool isActive; //checks if savedState is set
    jmp_buf savedState;
    File* file;
} ErrorHandler;


typedef struct {
    ArrayList tokens; 
    Token currentToken; 
    unsigned int tokenIndex; //index of the current token
    unsigned int indentationLevel;
    ErrorHandler err;
} Parser;


void parser_create(Parser *p, File* f, ArrayList tokens);

Token parser_next_token(Parser *p);

Token parser_prev_token(Parser* p);

Token parser_peek_token(Parser *p);


//consumes the current token and returns the next one 
//ensures current token is equal to expected
Token parser_consume_token(Parser *p, TokenType expected, const char* fmt, ...);


//ensures the current token is equal to expected 
//unlike parser_consume_token this function does not 
//change the value of current token 
void parser_expect_token(Parser *p, TokenType expected);

//just calls parser_next_token()
//makes it more clear which token we are consuming 
Token parser_consume_verified_token(Parser *p, TokenType token);


Token parser_expect_consume_token(Parser *p, TokenType expected);

//if the current token == tok, we consume the current token 
//else do nothing 
void parser_try_consume_token(Parser *p, TokenType tok);

noreturn void parser_new_error(Parser *p, const char* fmt, ...);


ArrayList parse_tokens(Parser *p);


//this should not be invoked directly 
//use the macro instead
bool __match(Parser *p, ...);

#define match(p,...) __match(p, __VA_ARGS__, TOK_MAX)

//checks to see if the next "level_count" tokens are TABS  
//if yes it consumes them and returns true else just returns false
bool parser_match_indentation_level(Parser *p, unsigned int level_count);









