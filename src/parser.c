#include "parser.h"
#include "arraylist.h"
#include "file.h"
#include "statement.h"
#include "tokenizer.h"
#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>



void parser_create(Parser *p, File* f, ArrayList* tokens){
    if(array_list_size(tokens) < 1){
        exit(0);
    }
    p->tokens = tokens; 
    p->currentToken = (Token){TOK_MAX, NULL};
    p->tokenIndex = 0;
    p->indentationLevel = 0;
    p->file = f;
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
    if(p->tokenIndex + 1 < p->tokens->size){
        p->tokenIndex++;
        Token res = array_list_get(p->tokens, Token, p->tokenIndex);
        p->currentToken = res;
        return res;

    }
    p->currentToken = (Token){TOK_EOF, NULL};  
    return p->currentToken;
}




Token parser_prev_token(Parser *p){ 
    if(p->tokenIndex > 0 && p->tokenIndex <= p->tokens->size){
        return array_list_get(p->tokens, Token, (p->tokenIndex - 1));
    }
    p->currentToken = (Token){TOK_EOF, NULL};  
    return (Token){TOK_EOF, NULL};
}


Token parser_consume_token(Parser *p, TokenType expected, const char* fmt, ...){
   if(p->currentToken.type != expected){
       //its fine if we come across an EOF instead of a new line
       //we treat them as essentially the same thing
       if(expected == TOK_NEW_LINE && p->currentToken.type == TOK_EOF){
            return (Token){TOK_EOF, NULL};
       }
        va_list list;
        va_start(list, fmt);
        fprintf(stderr, "Error: \n");
        file_eprint_line(p->file, p->currentToken.line);
        vfprintf(stderr, fmt, list);
        va_end(list);
        exit(EXIT_FAILURE);
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
        parser_new_error(p, "Error expected %s found %s\n",get_token_type(expected), get_token_type(p->currentToken.type));
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
    file_eprint_line(p->file, p->currentToken.line);
    va_list list;
    va_start(list, fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);
    exit(EXIT_FAILURE);
}



Token parser_peek_token(Parser *p){
    if(p->tokenIndex + 1 < p->tokens->size){
        return array_list_get(p->tokens, Token, p->tokenIndex + 1);
    }
    return (Token){TOK_EOF, NULL};  
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



ArrayList* parse_tokens(Parser *p){
    ArrayList* statements;
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



