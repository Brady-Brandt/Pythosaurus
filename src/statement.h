#pragma once
#include "arraylist.h"
#include "parser.h"
#include "stringtype.h"
#include "expression.h"
#include "tokenizer.h"

/*
stmt = FunctionDef(identifier name, arguments args,
                       stmt* body, expr* decorator_list, expr? returns)

          | ClassDef(identifier name,
             expr* bases,
             keyword* keywords,
             stmt* body,
             expr* decorator_list)
          | Return(expr? value)

          | Delete(expr* targets)

          | For(expr target, expr iter, stmt* body, stmt* orelse)
          | If(expr test, stmt* body, stmt* orelse)
          | With(withitem* items, stmt* body)

          | Raise(expr? exc, expr? cause)
          | Try(stmt* body, excepthandler* handlers, stmt* orelse, stmt* finalbody)
          | Assert(expr test, expr? msg)

          | Import(alias* names)
          | ImportFrom(identifier? module, alias* names, int? level)

          | Global(identifier* names)
          | Nonlocal(identifier* names)
          | Expr(expr value)
          | Pass | Break | Continue
*/

typedef enum {
    BLOCK_NONE, 
    BLOCK_CONDITIONAL, 
    BLOCK_LOOP,
    BLOCK_FUNCTION, 
    BLOCK_CLASS, 
} BlockType;


typedef enum {
    STMT_ASSIGN,
    STMT_ASSIGN_OP,
    STMT_EXPR,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
    STMT_FUNC,
    STMT_RETURN, 
    STMT_PASS,
}StatementType;

typedef struct {
    StatementType type;
    void* stmt;
} Statement;

typedef struct {
    String identifier;
    Expr* value;
} AssignStmt; 

typedef struct {
    String identifier;
    TokenType op;
    Expr* value;
} AssignOpStmt; 


typedef struct {
    Expr* expr;
} ExprStmt;


typedef struct {
    BlockType type;
    ArrayList statements;
} BlockStmt;

typedef struct {
    Expr* condition;
    Statement* then;
    Statement* elseBranch;
} IfStmt; 


typedef struct {
    Expr* condition;
    Statement* _while;
} WhileStmt;

typedef struct {
    String identifier;
    ArrayList parameters; //type token 
    Statement* body;
} FunctionStmt;

typedef struct {
    Expr* value;
} ReturnStmt;


Statement* statement(Parser *p);
