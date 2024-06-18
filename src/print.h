#pragma once 


#include "arraylist.h"
#include "tokenizer.h"
#include "expression.h"
#include "statement.h"

void print_token(Token token);

void print_tokens(ArrayList tokens);

const char* get_token_type(TokenType t);

void print_expr(Expr* expression);

void print_statement(Statement* stmt);


