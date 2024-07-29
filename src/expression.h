#pragma once 

#include "tokenizer.h"
#include "parser.h"

/*
expr = BoolOp(boolop op, expr* values)
         | BinOp(expr left, operator op, expr right)
         | UnaryOp(unaryop op, expr operand)
         | Lambda(arguments args, expr body)
         | IfExp(expr test, expr body, expr orelse)
         | Dict(expr* keys, expr* values)
         | Set(expr* elts)
         | ListComp(expr elt, comprehension* generators)
         | SetComp(expr elt, comprehension* generators)
         | DictComp(expr key, expr value, comprehension* generators)
         | GeneratorExp(expr elt, comprehension* generators)
         -- the grammar constrains where yield expressions can occur
         | Yield(expr? value)
         | YieldFrom(expr value)
         -- need sequences for compare to distinguish between
         -- x < 4 < 3 and (x < 4) < 3
         | Compare(expr left, cmpop* ops, expr* comparators)
         | Call(expr func, expr* args, keyword* keywords)
         | Num(object n) -- a number as a PyObject.
         | Str(string s) -- need to specify raw, unicode, etc?
         | Bytes(bytes s)
         | NameConstant(singleton value)
         | Ellipsis
*/


typedef enum {
    EXPR_BINARY, 
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_FUNC, 
    EXPR_GROUPING,
} ExprType;


typedef enum {
    LIT_BOOL,
    LIT_INTEGER, 
    LIT_FLOAT, 
    LIT_STRING,
    LIT_IDENTIFIER,
    LIT_CLASS,
    LIT_LIST,
    LIT_DICT,
    LIT_NONE,
    LIT_UNINIT,
} LiteralType;

typedef struct {
    ExprType type;
    void* expr;
} Expr;

typedef struct {
    ExprType type;
    TokenType op;
    Expr* right;
} UnaryExpr;

typedef struct {
    ExprType type;
    Expr* left;
    TokenType op; 
    Expr* right;
} BinaryExpr;


typedef struct {
    ExprType type;
    LiteralType litType;
    union {
        String* string;
        long integer;
        double _float;
        String* identifier;
        bool boolean;
    };
} LiteralExpr;


extern LiteralExpr __none_expr__;

#define None_Expr (&__none_expr__)

//represents a function call
typedef struct {
    ExprType type;
    String* name;
    ArrayList* args; //arraylist of expressions 
} FuncExpr;


typedef struct { 
    ExprType type;
    Expr* expr;
} GroupingExpr;


//creates an expression using recursive descent 
Expr* expression(Parser *p);

void delete_literal_expr(LiteralExpr* expr);

void delete_expr_tree(Expr* expr);




