#include "parser.h"
#include "arraylist.h"
#include "file.h"
#include "statement.h"
#include "stringtype.h"
#include "tokenizer.h"
#include "print.h"

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>



void parser_create(Parser *p, File* f, ArrayList tokens){
    if(tokens.size < 1){
        fprintf(stderr, "Invalid token count\n");
        exit(1);
    }
    p->tokens = tokens; 
    p->currentToken = (Token){TOK_MAX, DEFAULT_STR};
    p->tokenIndex = 0;
    p->indentationLevel = 0;
    p->err.file = f;
    p->err.isActive = false;
}


noreturn static void parser_throw_error(Parser *p){
    if(p->err.isActive){ 
        longjmp(p->err.savedState, PARSER_ERROR);
    } else {
        fprintf(stderr, "Unrecoverabled error, exiting...");
        exit(EXIT_FAILURE);
    }
}


static void parser_delete(Parser *p){
    p->currentToken.type = TOK_EOF;
    p->tokenIndex = 0;
    array_list_delete(p->tokens);
}





Token parser_next_token(Parser *p){
    if(p->tokenIndex == 0 && p->currentToken.type == TOK_MAX){
        Token res = array_list_get(p->tokens, Token, p->tokenIndex);
        p->currentToken = res;
        return res;
    }
    if(p->tokenIndex + 1 < p->tokens.size){
        p->tokenIndex++;
        Token res = array_list_get(p->tokens, Token, p->tokenIndex);
        p->currentToken = res;
        return res;

    }
    return (Token){TOK_EOF, DEFAULT_STR};  
}




Token parser_prev_token(Parser *p){ 
    if(p->tokenIndex > 0 && p->tokenIndex <= p->tokens.size){
        return array_list_get(p->tokens, Token, (p->tokenIndex - 1));
    }
    return (Token){TOK_EOF, DEFAULT_STR};
}


Token parser_consume_token(Parser *p, TokenType expected, const char* fmt, ...){
   if(p->currentToken.type != expected){
       //its fine if we come across an EOF instead of a new line
       //we treat them as essentially the same thing
       if(expected == TOK_NEW_LINE && p->currentToken.type == TOK_EOF){
            return (Token){TOK_EOF, DEFAULT_STR};
       }
        va_list list;
        va_start(list, fmt);
        fprintf(stderr, "Error: \n");
        file_eprint_line(p->err.file, p->currentToken.line);
        vfprintf(stderr, fmt, list);
        va_end(list);
        parser_throw_error(p);
   }
   return parser_next_token(p);
}


void parser_expect_token(Parser *p, TokenType expected){
    if(p->currentToken.type != expected){
        //its fine if we come across an EOF instead of a new line
        //we treat them as essentially the same thing
        if(expected == TOK_NEW_LINE && p->currentToken.type == TOK_EOF){
            return;
        }
        fprintf(stderr, "Error: \n");
        file_eprint_line(p->err.file, p->currentToken.line);
        fprintf(stderr, "Error expected %s found %s\n", get_token_type(expected), get_token_type(p->currentToken.type));
        parser_throw_error(p);
    }
}



Token parser_expect_consume_token(Parser *p, TokenType expected){
    parser_expect_token(p, expected);
    return parser_next_token(p);

}


Token parser_consume_verified_token(Parser *p, TokenType _token){
    return parser_next_token(p);
}

void parser_try_consume_token(Parser *p, TokenType tok){
    if(p->currentToken.type == tok){
        parser_next_token(p);
    }
}



noreturn void parser_new_error(Parser *p, const char* fmt, ...){
    fprintf(stderr, "Error: \n");
    file_eprint_line(p->err.file, p->currentToken.line);
    va_list list;
    va_start(list, fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);
    parser_throw_error(p);
}



Token parser_peek_token(Parser *p){
    if(p->tokenIndex + 1 < p->tokens.size){
        return array_list_get(p->tokens, Token, p->tokenIndex + 1);
    }
    return (Token){TOK_EOF, DEFAULT_STR};  
}



bool parser_match_indentation_level(Parser *p, unsigned int level_count){
    //saving the state just incase indentationLevel doesn't match level count 
    Token savedToken = p->currentToken; //this should be a tab  
    unsigned int savedTokenIndex = p->tokenIndex;
    for(int i = 0; i < level_count; i++){
        if(p->currentToken.type != TOK_TAB){
            p->currentToken = savedToken;
            p->tokenIndex = savedTokenIndex;
            return false;
        }
        parser_next_token(p);
    }
    return true;
}





bool __match(Parser *p, ...){
    va_list list;
    va_start(list, p);

    while(true){
        TokenType current = va_arg(list, TokenType);
        if(current == TOK_MAX) break;

        if(current == p->currentToken.type){ 
            parser_next_token(p);
            va_end(list);
            return true;
        }
    }
    va_end(list);
    return false;
}

void parser_save_state(Parser *p, struct ParserState* pstate){
    pstate->saved = p->currentToken;
    pstate->index = p->tokenIndex;
}

void parser_restore_state(Parser *p, struct ParserState* pstate){
    if(pstate == NULL) return;
    p->currentToken = pstate->saved;
    p->tokenIndex = pstate->index;
}



ArrayList parse_tokens(Parser *p){
    ArrayList statements;
    array_list_create(statements, Statement*);
    parser_next_token(p);

    while(p->currentToken.type != TOK_EOF){ 
        Statement* stmt = statement(p);
        if(stmt == NULL){
            if(p->currentToken.type == TOK_EOF){
                break;
            }
            parser_new_error(p, "Unexpected statement");
        }
        array_list_append(statements, Statement*, stmt);
    } 
    parser_delete(p);
    return statements;
}



