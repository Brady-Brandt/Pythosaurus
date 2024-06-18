#include "statement.h"
#include "arraylist.h"
#include "parser.h"
#include "expression.h"
#include "tokenizer.h"


#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>


Statement* create_assign_stmt(String id, Expr* value){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_ASSIGN;
    result->stmt = malloc(sizeof(AssignStmt));
    if(result->stmt != NULL){
        AssignStmt* temp = (AssignStmt*)result->stmt;
        temp->identifier = id;
        temp->value = value;
    }
    return result;
}



Statement* create_op_assign_stmt(String id, TokenType operator, Expr* value){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_ASSIGN_OP;

    result->stmt = malloc(sizeof(AssignOpStmt));
    if(result->stmt != NULL){
        AssignOpStmt* temp = (AssignOpStmt*)result->stmt;
        temp->identifier = id;
        temp->value = value;
        temp->op = operator;
    }
    return result;

}


Statement* create_expr_stmt(Expr* expr){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_EXPR;
    result->stmt = malloc(sizeof(ExprStmt)); 
    if(result->stmt != NULL){
        ExprStmt* temp = (ExprStmt*)result->stmt;
        temp->expr = expr; 
    }
    return result;

}


Statement* create_block_stmt(ArrayList statements){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_BLOCK;
    result->stmt = malloc(sizeof(BlockStmt)); 
    if(result->stmt != NULL){
        BlockStmt* temp = (BlockStmt*)result->stmt;
        temp->statements= statements; 
    }
    return result;
}

Statement* create_if_stmt(Expr* condition, Statement* then, Statement* elseBranch){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_IF;
    result->stmt = malloc(sizeof(IfStmt)); 
    if(result->stmt != NULL){
        IfStmt* temp = (IfStmt*)result->stmt;
        temp->condition = condition;
        temp->then = then;
        temp->elseBranch = elseBranch;
    }
    return result;
}


Statement* create_while_stmt(Expr* condition, Statement* _while){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_WHILE;
    result->stmt = malloc(sizeof(WhileStmt)); 
    if(result->stmt != NULL){
        WhileStmt* temp = (WhileStmt*)result->stmt;
        temp->condition = condition;
        temp->_while = _while;
    }
    return result;
}


Statement* create_func_stmt(String identifier, ArrayList parameters, Statement* body){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_FUNC;
    result->stmt = malloc(sizeof(FunctionStmt)); 
    if(result->stmt != NULL){
        FunctionStmt* temp = (FunctionStmt*)result->stmt;
        temp->identifier = identifier;
        temp->parameters = parameters;
        temp->body = body;
    }
    return result;
}



Statement* create_return_stmt(Expr* value){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_RETURN;
    result->stmt = malloc(sizeof(ReturnStmt)); 
    if(result->stmt != NULL){
        ReturnStmt* temp = (ReturnStmt*)result->stmt;
        temp->value = value;
    }
    return result;
}

Statement* create_pass_stmt(){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_PASS;
    result->stmt = NULL; 
    return result;
}




Statement* assign_statement(Parser* p){
    switch (parser_peek_token(p).type) {
        case TOK_ASSIGN: {
            String identifier = p->currentToken.literal;
            parser_consume_verified_token(p, TOK_IDENTIFIER);
            parser_consume_verified_token(p, TOK_ASSIGN);
            Expr* value = expression(p);
            Statement* stmt = create_assign_stmt(identifier, value); //TODO: ADD SUPORT FOR SEMICOLONS TO TERMINATE 
            //STATEMENTS
            parser_expect_token(p, TOK_NEW_LINE);
            return stmt;
        }
        case TOK_ADD_ASSIGN:
        case TOK_SUB_ASSIGN:
        case TOK_MUL_ASSIGN:
        case TOK_DIV_ASSIGN:
        case TOK_MOD_ASSIGN:
        case TOK_EXP_ASSIGN:
        case TOK_FLOOR_DIV_ASSIGN:
        case TOK_LEFT_SHIFT_ASSIGN:
        case TOK_RIGHT_SHIFT_ASSIGN:
        case TOK_BIT_AND_ASSIGN:
        case TOK_BIT_OR_ASSIGN:
        case TOK_BIT_XOR_ASSIGN:{
            String identifier = p->currentToken.literal;  
            Token operation = parser_consume_verified_token(p, TOK_IDENTIFIER);
            //skip over the op
            parser_next_token(p);
            Expr* value = expression(p);
            Statement* stmt = create_op_assign_stmt(identifier, operation.type, value);
            parser_expect_token(p, TOK_NEW_LINE);
            return stmt; 
        }
        default:{
            Expr* value = expression(p);
            parser_expect_token(p, TOK_NEW_LINE);
            return create_expr_stmt(value);
        } 
    }      
}



Statement* block_statement(Parser* p, unsigned int expected_indent){
    p->indentationLevel++;
    ArrayList statements;
    array_list_create_cap(statements, Statement*, 10);
    bool is_correct_indent = parser_match_indentation_level(p, expected_indent);

    //each block must contain at least one statement 
    if(!is_correct_indent){
        parser_new_error(p,"Expected at least one statement inside block\n");
        fprintf(stderr, "Invalid indentation level\n");
        exit(1);
    }

    while(is_correct_indent){
       Statement* curr_stmt = statement(p);
       array_list_append(statements, Statement*, curr_stmt); 
       parser_try_consume_token(p, TOK_NEW_LINE);
       is_correct_indent = parser_match_indentation_level(p, expected_indent);
    }

    p->indentationLevel--;
    return create_block_stmt(statements);
}



Statement* if_statement(Parser *p){
    unsigned int expected_indent = p->indentationLevel + 1;
    parser_consume_verified_token(p, TOK_IF);
    Expr* condition = expression(p);
    parser_expect_consume_token(p, TOK_COLON);
    parser_expect_consume_token(p, TOK_NEW_LINE);
    Statement* then = block_statement(p, expected_indent);
    Statement* elseBranch = NULL;
    parser_try_consume_token(p, TOK_NEW_LINE);


    //make sure the else clause is properly aligned with the top level else 
    bool is_correct_indent = parser_match_indentation_level(p, expected_indent - 1);
    if(is_correct_indent && match(p,TOK_ELSE)){
        parser_expect_consume_token(p, TOK_COLON);
        parser_expect_consume_token(p, TOK_NEW_LINE);
        elseBranch = block_statement(p, expected_indent);
    }     
    return create_if_stmt(condition, then, elseBranch);
}


Statement* while_statement(Parser *p){
    unsigned int expected_indent = p->indentationLevel + 1;
    parser_consume_verified_token(p, TOK_WHILE);
    Expr* cond = expression(p);
    parser_expect_consume_token(p, TOK_COLON);
    parser_expect_consume_token(p, TOK_NEW_LINE);
    Statement* _while = block_statement(p, expected_indent);
    return create_while_stmt(cond, _while);
}



Statement* function_statement(Parser *p){
    parser_consume_verified_token(p, TOK_DEF);
    parser_expect_consume_token(p, TOK_IDENTIFIER);
    String identifier = parser_prev_token(p).literal;
    parser_expect_consume_token(p, TOK_LEFT_PAREN);
    ArrayList parameters;
    array_list_create_cap(parameters, String, 4);

    //TODO: VARADIC AND KEYWORD ARGS 
    if(p->currentToken.type != TOK_RIGHT_PAREN){
       do {
            
           parser_expect_token(p, TOK_IDENTIFIER);
           array_list_append(parameters, String, p->currentToken.literal);
           parser_next_token(p);
       }while (match(p, TOK_COMMA)); 
    }

    parser_expect_consume_token(p, TOK_RIGHT_PAREN);
    parser_expect_consume_token(p, TOK_COLON);
    parser_expect_consume_token(p, TOK_NEW_LINE);
    unsigned int expected_indent = p->indentationLevel + 1;
    Statement* body = block_statement(p, expected_indent);
    return create_func_stmt(identifier, parameters, body);
}



Statement* return_statement(Parser *p){
    parser_next_token(p);
    Expr* value = NULL;
    if(p->currentToken.type != TOK_NEW_LINE){
        value = expression(p); 
    }
    return create_return_stmt(value);
}


Statement* pass_statement(Parser *p){
    parser_next_token(p);
    parser_expect_token(p, TOK_NEW_LINE);
    return create_pass_stmt();
}


Statement* statement(Parser *p){
    parser_try_consume_token(p, TOK_NEW_LINE);
    p->err.isActive = true;
    if(setjmp(p->err.savedState) != PARSER_ERROR){
        switch(p->currentToken.type){
            case TOK_IDENTIFIER:
                return assign_statement(p);
            case TOK_IF:
                return if_statement(p);
            case TOK_WHILE:
                return while_statement(p);
            case TOK_DEF:
                return function_statement(p);
            case TOK_RETURN:
                return return_statement(p);
            case TOK_PASS:
                return pass_statement(p);
            default:
                parser_next_token(p);
                return NULL;
        }
    } else {
        exit(EXIT_FAILURE);
    } 
}


