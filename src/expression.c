#include <stdlib.h>

#include "expression.h"
#include "arraylist.h"
#include "parser.h"
#include "stringtype.h"
#include "tokenizer.h"


static Expr* create_binary_expr(Expr* left, TokenType operator,Expr* right){
   Expr* result = malloc(sizeof(Expr));
   if(result == NULL) return NULL;
   result->type = EXPR_BINARY; 
   result->expr = malloc(sizeof(BinaryExpr));
   if(result->expr != NULL){
       BinaryExpr* temp = (BinaryExpr*)result->expr;
       temp->left = left;
       temp->op = operator;
       temp->right = right;
   }      
   return result;
}



static Expr* create_unary_expr(TokenType operator, Expr* right){
   Expr* result = malloc(sizeof(Expr));
   if(result == NULL) return NULL;
   result->type = EXPR_UNARY; 
   result->expr = malloc(sizeof(UnaryExpr));
   if(result->expr != NULL){
       UnaryExpr* temp = (UnaryExpr*)result->expr;
       temp->op = operator;
       temp->right = right;
   }      
   return result;

}


static Expr* create_func_expr(String name, ArrayList args){
    Expr* result = malloc(sizeof(Expr));
    if(result == NULL) return NULL;
    result->type = EXPR_FUNC;
    result->expr = malloc(sizeof(FuncExpr));
    if(result->expr != NULL){
        FuncExpr* temp = (FuncExpr*)result->expr;
        temp->args = args;
        temp->name = name;
    }
    return result;
}


static Expr* create_grouping_expr(Expr* e){
    Expr* result = malloc(sizeof(Expr));
    if(result == NULL) return NULL;
    result->type = EXPR_GROUPING;
    result->expr = e;
    return result;
}


static Expr* create_logical_expr(Expr* left, TokenType operator,Expr* right){
   Expr* result = malloc(sizeof(Expr));
   if(result == NULL) return NULL;
   result->type = EXPR_LOGICAL; 
   result->expr = malloc(sizeof(LogicalExpr));
   if(result->expr != NULL){
       LogicalExpr* temp = (LogicalExpr*)result->expr;
       temp->left = left;
       temp->op = operator;
       temp->right = right;
   }      
   return result;
}


static Expr* primary(Parser *p){
    //check for a group expression first 
    if(match(p, TOK_LEFT_PAREN)){
       Expr* e = expression(p); 
       parser_consume_token(p, TOK_RIGHT_PAREN, "Expected closing ')'\n");
       return create_grouping_expr(e);
    }

    //it should be a literal expression here
    Expr* result = malloc(sizeof(Expr));
    if(result == NULL) return NULL;
    result->type = EXPR_LITERAL;  \
    result->expr = (LiteralExpr*)malloc(sizeof(LiteralExpr)); \
    if(result->expr == NULL){
        free(result);
        return NULL;
    }
    LiteralExpr* temp = (LiteralExpr*)result->expr;
    switch (p->currentToken.type) {
        case TOK_IDENTIFIER: 
            temp->litType = LIT_IDENTIFIER;
            temp->identifier = p->currentToken.literal;
            break;
        case TOK_STRING:
            temp->litType = LIT_STRING;
            temp->string = p->currentToken.literal;
            break;

        case TOK_INTEGER:
            temp->litType = LIT_INTEGER;
            temp->integer = strtol(p->currentToken.literal.str, NULL, 10);
            string_delete(&p->currentToken.literal);
            break;
        
        case TOK_FLOAT:
            temp->litType = LIT_FLOAT;
            temp->_float = strtod(p->currentToken.literal.str, NULL);
            string_delete(&p->currentToken.literal);
            break;

        case TOK_TRUE:
        case TOK_FALSE:
            temp->litType = LIT_BOOL;
            temp->boolean = p->currentToken.type == TOK_TRUE;
            break;
        case TOK_NONE:
            temp->litType = LIT_NONE;
            temp->none = NULL;
            break; 
        default:
            free(temp);
            free(result);
            parser_new_error(p, "Invalid expression\n"); 
    }
    parser_next_token(p);
    return result;
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

        LiteralExpr* lexpr = (LiteralExpr*)expr->expr;
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
    while (match(p,TOK_GREATER_THAN,TOK_GREATER_EQUAL,TOK_LESS_EQUAL,TOK_LESS_THAN,TOK_EQUAL,TOK_LESS_THAN, TOK_IN, TOK_IS)) {
      TokenType operator = parser_prev_token(p).type; 
      Expr* right = bit_or(p);
      expr = create_binary_expr(expr, operator, right);
    }
    return expr; 
}



static Expr* not(Parser *p){
    Expr* expr = comparison(p);
    while(match(p,TOK_AND)){
        TokenType op = parser_prev_token(p).type;
        expr = create_logical_expr(expr, op, NULL);
    }
    return expr;
}

static Expr* and(Parser *p){
    Expr* expr = not(p);
    while(match(p,TOK_AND)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = not(p);
        expr = create_logical_expr(expr, op, right);
    }
    return expr;
}



static Expr* or(Parser *p){
    Expr* expr = and(p);
    while(match(p,TOK_OR)){
        TokenType op = parser_prev_token(p).type;
        Expr* right = and(p);
        expr = create_logical_expr(expr, op, right);
    }
    return expr;
}


Expr* expression(Parser* p){
    return or(p); 
}


