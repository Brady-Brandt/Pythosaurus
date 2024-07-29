#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "expression.h"
#include "arraylist.h"
#include "parser.h"
#include "print.h"
#include "stringtype.h"
#include "tokenizer.h"



LiteralExpr __none_expr__ = (LiteralExpr){EXPR_LITERAL, LIT_NONE};


static Expr* create_binary_expr(Expr* left, TokenType operator,Expr* right){
    BinaryExpr* result = malloc(sizeof(BinaryExpr));
    if(result == NULL) return NULL;
    result->type = EXPR_BINARY; 
    result->left = left;
    result->op = operator;
    result->right = right;
    return (Expr*)result;
}



static Expr* create_unary_expr(TokenType operator, Expr* right){
    UnaryExpr* result = malloc(sizeof(UnaryExpr));
    if(result == NULL) return NULL;
    result->type = EXPR_UNARY;
    result->op = operator;
    result->right = right;
    return (Expr*)result;
}


static Expr* create_func_expr(String* name, ArrayList args){
    FuncExpr* result = malloc(sizeof(FuncExpr));
    if(result == NULL) return NULL;
    result->type = EXPR_FUNC;
    result->args = args;
    result->name = name; 
    return (Expr*)result;
}


static Expr* create_grouping_expr(Expr* e){
    GroupingExpr* result = malloc(sizeof(GroupingExpr));
    if(result == NULL) return NULL;
    result->type = EXPR_GROUPING;
    result->expr = e;
    return (Expr*)result;
}



static Expr* primary(Parser *p){
    //check for a group expression first 
    if(match(p, TOK_LEFT_PAREN)){
       Expr* e = expression(p); 
       parser_consume_token(p, TOK_RIGHT_PAREN, "Expected closing ')'\n");
       return create_grouping_expr(e);
    }
    if(p->currentToken.type == TOK_NONE){
        parser_next_token(p);
        return (Expr*)None_Expr;
    }

    //it should be a literal expression here 
    LiteralExpr* result = (LiteralExpr*)malloc(sizeof(LiteralExpr));
    result->type = EXPR_LITERAL;
    switch (p->currentToken.type) {
        case TOK_IDENTIFIER: 
            result->litType = LIT_IDENTIFIER;
            result->identifier = p->currentToken.literal;
            break;
        case TOK_STRING:
            result->litType = LIT_STRING;
            result->string = p->currentToken.literal;
            break;

        case TOK_INTEGER:
            result->litType = LIT_INTEGER;
            errno = 0;
            result->integer = strtol(p->currentToken.literal->str, NULL, 10);
            if(errno == ERANGE){
                parser_new_error(p, "Invalid integer: %s\n", p->currentToken.literal->str);
            }
            string_delete(p->currentToken.literal);
            break;
        
        case TOK_FLOAT:
            result->litType = LIT_FLOAT;
            errno = 0;
            result->_float = strtod(p->currentToken.literal->str, NULL);
            if(errno == ERANGE){
                parser_new_error(p, "Invalid float: %s\n", p->currentToken.literal->str);
            }
            string_delete(p->currentToken.literal);
            break;

        case TOK_TRUE:
        case TOK_FALSE:
            result->litType = LIT_BOOL;
            result->boolean = p->currentToken.type == TOK_TRUE;
            break;
        default:
            free(result);
            parser_new_error(p, "Invalid expression: %s\n", get_token_type(p->currentToken.type)); 
    }
    parser_next_token(p);
    return (Expr*)result;
}



static ArrayList func_arg(Parser *p){
    //the function has no parameters 
    //return emtpy arg list 
    if(p->currentToken.type == TOK_RIGHT_PAREN){
        parser_next_token(p);
        return DEFAULT_ARRAY_LIST;
    }

    ArrayList args;
    array_list_create_cap(args, Expr*, 4);

    do {
       Expr* arg = expression(p);
       array_list_append(args, Expr*, arg);
    }while(match(p, TOK_COMMA));

    parser_consume_token(p, TOK_RIGHT_PAREN, "Missing closing ')'");

    return args;
}


static Expr* call(Parser *p){
   Expr* expr = primary(p);
   while(match(p, TOK_LEFT_PAREN)){
        ArrayList args = func_arg(p);
        if(expr->type != EXPR_LITERAL){
            parser_new_error(p, "Invalid function name\n");
        }

        LiteralExpr* lexpr = (LiteralExpr*)expr;
        if(lexpr->litType != LIT_IDENTIFIER){ 
            parser_new_error(p, "Invalid function name\n");
        }

        return create_func_expr(lexpr->string, args);
   }
    return expr;
}

static Expr* exp(Parser *p){
    Expr* expr = call(p);
    while(match(p, TOK_EXP)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = call(p);
        return create_binary_expr(expr, op, right);
    }
    return expr;
}


static Expr* unary(Parser *p){
    while(match(p, TOK_ADD, TOK_SUB, TOK_BIT_NOT)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = unary(p);
        return create_unary_expr(op, right);
    }
    return exp(p);
}


static Expr* factor(Parser *p){
   Expr* expr = unary(p); 
    while (match(p,TOK_MUL,TOK_DIV,TOK_MOD,TOK_FLOOR_DIV)) {
      TokenType operator = parser_prev_token(p).type;
      Expr* right = unary(p);
      expr = create_binary_expr(expr, operator, right);
    }
    return expr; 
}


static Expr* term(Parser *p){
    Expr* expr = factor(p);
    while (match(p,TOK_SUB,TOK_ADD)) {
      TokenType operator = parser_prev_token(p).type;
      Expr* right = factor(p);
      expr = create_binary_expr(expr, operator,right);
    }
    return expr;
}


static Expr* bit_shift(Parser *p){
    Expr* expr = term(p);
    while(match(p, TOK_LEFT_SHIFT, TOK_RIGHT_SHIFT)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = term(p);
        expr = create_binary_expr(expr, op, right);
    }
    return expr;
}


static Expr* bit_and(Parser *p){
    Expr* expr = bit_shift(p);
    while(match(p, TOK_BIT_AND)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = bit_shift(p);
        expr = create_binary_expr(expr, op, right);
    }
    return expr;
}

static Expr* bit_xor(Parser *p){
    Expr* expr = bit_and(p);
    while(match(p, TOK_BIT_XOR)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = bit_and(p);
        expr = create_binary_expr(expr, op, right);
    }
    return expr;
}


static Expr* bit_or(Parser *p){
    Expr* expr = bit_xor(p);
    while(match(p, TOK_BIT_OR)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = bit_xor(p);
        expr = create_binary_expr(expr, op, right);
    }
    return expr;
}

static Expr* comparison(Parser *p){ 
    Expr* expr = bit_or(p); 
    while (match(p,TOK_GREATER_THAN,TOK_GREATER_EQUAL,TOK_LESS_EQUAL,TOK_LESS_THAN,TOK_EQUAL,TOK_NOT_EQUAL,TOK_LESS_THAN, TOK_IN, TOK_IS)) {
      TokenType operator = parser_prev_token(p).type; 
      Expr* right = bit_or(p);
      expr = create_binary_expr(expr, operator, right);
    }
    return expr; 
}



static Expr* not(Parser *p){
    while(match(p,TOK_NOT)){
        TokenType op = parser_prev_token(p).type;
        Expr* expr = comparison(p);
        return create_unary_expr(op, expr); 
    }
    return comparison(p);
}

static Expr* and(Parser *p){
    Expr* expr = not(p);
    while(match(p,TOK_AND)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = not(p);
        expr = create_binary_expr(expr, op, right);
    }
    return expr;
}



static Expr* or(Parser *p){
    Expr* expr = and(p);
    while(match(p,TOK_OR)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = and(p);
        expr = create_binary_expr(expr, op, right);
    }
    return expr;
}


Expr* expression(Parser* p){
    return or(p); 
}



void delete_expr_tree(Expr* expression){
    if(expression == NULL) return;
    switch (expression->type) {
        case EXPR_LITERAL: {
            LiteralExpr* lexpr = (LiteralExpr*)expression;
            if(lexpr->litType != LIT_NONE){
                if(lexpr->litType == LIT_STRING || lexpr->litType == LIT_IDENTIFIER){
                    string_delete(lexpr->string);
                }
                free(expression);
            }
            break;
        }
        case EXPR_BINARY:{
            BinaryExpr* bexpr = (BinaryExpr*)expression;
            if(bexpr->left != NULL){
               delete_expr_tree(bexpr->left); 
            }
            if(bexpr->right != NULL){
                delete_expr_tree(bexpr->right); 
            }
            free(bexpr);
            expression = NULL;
            break;
        } 
        case EXPR_GROUPING:
            delete_expr_tree(expression->expr);
            break;

        case EXPR_UNARY:{
            UnaryExpr* uexpr = (UnaryExpr*)expression; 
            if(uexpr->right != NULL){
                free(uexpr->right);
            }
            free(uexpr);
            break;
        }

        case EXPR_FUNC: {
            FuncExpr* fexpr = (FuncExpr*)expression; 
            for(int i = 0; i < fexpr->args.size; i++){
                Expr* e = array_list_get(fexpr->args, Expr*, i);
                delete_expr_tree(e);
            }

            string_delete(fexpr->name);
            array_list_delete(fexpr->args);
            free(fexpr);
            break;
        }
 
        default:
            break;
    }
}
