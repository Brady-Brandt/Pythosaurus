#include "print.h"
#include "arraylist.h"
#include "expression.h"
#include "statement.h"
#include "stringtype.h"
#include <stdio.h>

static const char* token_strings[] = {
    "false",
    "none",
    "true",
    "and",
    "as",
    "assert",
    "break",
    "class",
    "continue",
    "def",
    "del",
    "elif",
    "else",
    "except",
    "finally",
    "for",
    "from",
    "global",
    "if",
    "import",
    "in",
    "is",
    "lambda",
    "nonlocal",
    "not",
    "or",
    "pass",
    "raise",
    "return",
    "try",
    "while",
    "with",
    "yield",

    "+",    // ADD
    "-",    // SUB
    "*",    // MUL
    "/",    // DIV
    "%",    // MOD
    "**",   // EXP
    "//",   // FLOOR_DIV
    "&",    // BIT_AND
    "|",    // BIT_OR
    "^",    // BIT_XOR
    "~",    // BIT_NOT
    "<<",   // LEFT_SHIFT
    ">>",   // RIGHT_SHIFT

    "<",    // LESS_THAN
    ">",    // GREATER_THAN
    "<=",   // LESS_EQUAL
    ">=",   // GREATER_EQUAL
    "==",   // EQUAL
    "!=",   // NOT_EQUAL
    "=",    // ASSIGN

    "+=",   // ADD_ASSIGN
    "-=",   // SUB_ASSIGN
    "*=",   // MUL_ASSIGN
    "/=",   // DIV_ASSIGN
    "%=",   // MOD_ASSIGN
    "**=",  // EXP_ASSIGN
    "//=",  // FLOOR_DIV_ASSIGN
    "<<=",  // LEFT_SHIFT_ASSIGN
    ">>=",  // RIGHT_SHIFT_ASSIGN
    "&=",   // BIT_AND_ASSIGN
    "|=",   // BIT_OR_ASSIGN
    "^=",   // BIT_XOR_ASSIGN

    "(",    // LEFT_PAREN
    ")",    // RIGHT_PAREN
    "{",    // LEFT_BRACE
    "}",    // RIGHT_BRACE
    "[",    // LEFT_BRACKET
    "]",    // RIGHT_BRACKET
    "'",    // SQUOTE
    "\"",   // DQUOTE

    "EOF",         // EOF_
    "TAB",         // TAB
    "NEW_LINE",    // NEW_LINE
    ",",           // COMMA
    ":",           // COLON
    ";",           // SEMICOLON
    ".",           // DOT
    "...",         // ELLIPSIS
    "@",           // AT

    "identifier",  // IDENTIFIER
    "integer",     // INTEGER
    "float",       // FLOAT
    "string",      // STRING

    "unknown",     // UNKNOWN
    "max",         // MAX
};




const char* get_token_type(TokenType t){
    return token_strings[t];
}



#if DEBUG 
void print_token(Token token){
    if(token.type == TOK_STRING || token.type == TOK_IDENTIFIER || token.type == TOK_INTEGER || token.type == TOK_FLOAT){
        printf("%s -> %s\n", token_strings[token.type], token.literal.str);
    } else if(token.type < 33){
        printf("Keyword: %s\n", token_strings[token.type]);
    } 
    else{
        printf("%s\n", token_strings[token.type]);
    }
}


void print_tokens(ArrayList tokens){
    for(int i = 0; i < tokens.size; i++){
        Token current_tok = array_list_get(tokens, Token, i);
        print_token(current_tok);
    }
}



void print_expr(Expr* expression){
    if(expression == NULL) return;
    switch (expression->type) {
        case EXPR_LITERAL: {
            LiteralExpr* lexpr = expression->expr;

            switch (lexpr->litType) {
            case LIT_STRING:
                printf("%s", lexpr->string.str);
                break;
            case LIT_FLOAT: 
                printf("%g", lexpr->_float);
                break;
            case LIT_INTEGER: 
                printf("%ld", lexpr->integer);
                break;
            case LIT_IDENTIFIER:
                printf("Not implemented yet");
                break;
            default:
                printf("None");
            }
            break;
        }
        case EXPR_BINARY:{
            BinaryExpr* bexpr = expression->expr;
            if(bexpr->left == NULL){
                return;
            }
            print_expr((Expr*)bexpr->left);
            printf(" %s ", get_token_type(bexpr->op));
            if(bexpr->right == NULL){
                return;
            }
            print_expr((Expr*)bexpr->right);
            break;
        }
        case EXPR_LOGICAL: {
            LogicalExpr* logexpr = expression->expr;
            if(logexpr->left == NULL){
                return;
            }
            print_expr((Expr*)logexpr->left);
            printf(" %s ", get_token_type(logexpr->op));
            if(logexpr->right == NULL){
                return;
            }
            print_expr((Expr*)logexpr->right);
            break;
        }
        case EXPR_GROUPING:
            printf("(");
            print_expr(expression->expr);
            printf(")");
            break;

        case EXPR_UNARY:{
            UnaryExpr* uexpr = expression->expr; 
            printf(" %s", get_token_type(uexpr->op));
            if(uexpr->right == NULL){
                printf("Invalid unary operand\n");
                return;
            }
            print_expr((Expr*)uexpr->right);
            break;
        }

        case EXPR_FUNC: {
            FuncExpr* fexpr = expression->expr;
            printf("Func: %s ", fexpr->name.str);
            if(fexpr->args.data == NULL){
                printf("No args");
            } 
            for(int i = 0; i < fexpr->args.size; i++){
                Expr* e = array_list_get(fexpr->args, Expr*, i);
                print_expr(e);
                if(i + 1 < fexpr->args.size){
                    printf(",");
                }
            }
            printf("\n");
            break;
        }
 
        default:
            break;
    }
}


void print_statement(Statement* statement){
    if(statement == NULL){
        return;
    }
    printf("Statement: ");
    switch (statement->type) {
        case STMT_ASSIGN: {
            AssignStmt* s = (AssignStmt*)(statement->stmt);
            printf("Var %s = ", s->identifier.str);
            print_expr(s->value);
            printf("\n");
            break;
        }
        case STMT_ASSIGN_OP: {
            AssignOpStmt* s = (AssignOpStmt*)(statement->stmt);
            printf("Var %s ", s->identifier.str);
            printf("%s ",get_token_type(s->op));
            print_expr(s->value);
            printf("\n");
            break;
        }
        case STMT_EXPR: {
            ExprStmt* s = (ExprStmt*)(statement->stmt);
            printf("Expression: ");
            print_expr(s->expr);
            printf("\n");
            break;
        }

        case STMT_BLOCK: {
            BlockStmt* s = (BlockStmt*)(statement->stmt);
            printf("Block: \n");
            for(int i =0; i < s->statements.size; i++){
                Statement* temp = array_list_get(s->statements, Statement*, i);
                print_statement(temp);
            }
            printf("End Block\n");
            break;
        }
        case STMT_IF: {
            IfStmt* s = (IfStmt*)(statement->stmt);
            printf("if ");
            print_expr(s->condition);
            printf("\n");
            print_statement(s->then);
            if(s->elseBranch != NULL){
                printf("else\n");
                print_statement(s->elseBranch);
            }
            break;
        }
        case STMT_WHILE: {
            WhileStmt* stmt = (WhileStmt*)(statement->stmt);
            printf("While ");
            print_expr(stmt->condition);
            printf("Do:\n");
            print_statement(stmt->_while);
            break;
        }
        case STMT_FUNC: {
            FunctionStmt* stmt = (FunctionStmt*)(statement->stmt);
            printf("FunctionDef: %s ", stmt->identifier.str);
            if(stmt->parameters.data == NULL){
                printf("No args");
            } 
            for(int i = 0; i < stmt->parameters.size; i++){
                String arg = array_list_get(stmt->parameters, String, i);
                printf("%s", arg.str);
                if(i + 1 < stmt->parameters.size){
                    printf(",");
                }
            }
            printf("\n");
            print_statement(stmt->body);
            break;
        } 
        case STMT_RETURN: {
            ReturnStmt* stmt = (ReturnStmt*)(statement->stmt);
            if(stmt->value != NULL){
                print_expr(stmt->value);
                printf("\n");
            }
            break;
        }
        case STMT_PASS:
            printf("Pass\n");
            break;
        default:
            return; 
    }

}
#else
void print_expr(Expr *expression){}
void print_statement(Statement *expression){}
void print_token(Token token){}
void print_tokens(ArrayList tokens) {}
#endif 







