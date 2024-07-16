#pragma once
#include "arraylist.h"
#include "parser.h"
#include "stringtype.h"
#include "expression.h"
#include "tokenizer.h"
#include <stdint.h>

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
    STMT_ASSERT,
    STMT_RETURN, 
    STMT_PASS,
    STMT_BREAK,
    STMT_CONTINUE, 
}StatementType;

typedef struct {
    StatementType type;
    uint32_t line;
    void* stmt;
} Statement;

typedef struct {
    StatementType type;
    uint32_t line;
    String* identifier;
    Expr* value;
} AssignStmt; 

typedef struct {
    StatementType type;
    uint32_t line;
    String* identifier;
    TokenType op;
    Expr* value;
} AssignOpStmt; 


typedef struct {
    StatementType type;
    uint32_t line;
    Expr* expr;
} ExprStmt;


typedef struct {
    StatementType type;
    uint32_t line;
    BlockType BlockType;
    ArrayList statements;
} BlockStmt;


//special kind of statement that can only be nested 
//inside of the if statement
typedef struct {
    Expr* condition;
    uint32_t line;
    Statement* then;
    struct ElifStmt* next;
} ElifStmt;

typedef struct {
    StatementType type;
    uint32_t line;
    Expr* condition;
    ElifStmt* elif;
    Statement* then;
    Statement* elseBranch;
} IfStmt; 


typedef struct {
    StatementType type;
    uint32_t line;
    Expr* condition;
    Statement* _while;
} WhileStmt;

typedef struct {
    StatementType type;
    uint32_t line;
    String* identifier;
    ArrayList parameters;
    Statement* body;
} FunctionStmt;


typedef struct {
    StatementType type;
    uint32_t line;
    Expr* condition;
    String* msg;
} AssertStmt;


typedef struct {
    StatementType type;
    uint32_t line;
    Expr* value;
} ReturnStmt;


Statement* statement(Parser *p);

void delete_statement(Statement* stmt);

