#include "parser.h"
#include "arraylist.h"
#include "statement.h"
#include "stringtype.h"
#include "tokenizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>




Parser parser_create(ArrayList tokens){
    Parser result;
    if(tokens.size < 1){
        fprintf(stderr, "Invalid token count\n");
        exit(1);
    }
    result.tokens = tokens; 
    result.currentToken = (Token){TOK_MAX, DEFAULT_STR};
    result.tokenIndex = 0;
    return result;
}


static void parser_delete(Parser *p){
    for(int i = 0; i < p->tokens.size; i++){
        Token curr_tok = array_list_get(p->tokens, Token, i);
        token_delete(&curr_tok);
    }

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


Token parser_consume_token(Parser *p, TokenType expected){
   if(p->currentToken.type != expected){
        fprintf(stderr, "Warning expected a certain token here\n");
        exit(1);
   }
   return parser_next_token(p);
}


Token parser_peek_token(Parser *p){
    if(p->tokenIndex + 1 < p->tokens.size){
        return array_list_get(p->tokens, Token, p->tokenIndex + 1);
    }
    return (Token){TOK_EOF, DEFAULT_STR};  
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



ArrayList parse_tokens(Parser *p){
    ArrayList statements;
    array_list_create(statements, Statement);
    while(parser_next_token(p).type != TOK_EOF){
        if(p->currentToken.type == TOK_IDENTIFIER){
                    
        }
        

    } 
    parser_delete(p);
    return statements;
}

