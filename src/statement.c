#include "statement.h"
#include "arraylist.h"
#include "parser.h"
#include "expression.h"
#include "print.h"
#include "stringtype.h"
#include "tokenizer.h"


#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>


Statement* create_assign_stmt(String id, Expr* value){
    AssignStmt* result = malloc(sizeof(AssignStmt));
    if(result == NULL) return NULL;
    result->type = STMT_ASSIGN;
    result->identifier = id;
    result->value = value; 
    return (Statement*)result;
}



Statement* create_op_assign_stmt(String id, TokenType operator, Expr* value){
    AssignOpStmt* result = malloc(sizeof(AssignOpStmt));
    if(result == NULL) return NULL;
    result->type = STMT_ASSIGN_OP;
    result->identifier = id;
    result->value = value;
    result->op = operator;
    return (Statement*)result;
}


Statement* create_expr_stmt(Expr* expr){
    ExprStmt* result = malloc(sizeof(ExprStmt));
    if(result == NULL) return NULL;
    result->type = STMT_EXPR;
    result->expr = expr; 
    return (Statement*)result;
}


Statement* create_block_stmt(ArrayList statements){
    BlockStmt* result = malloc(sizeof(BlockStmt));
    if(result == NULL) return NULL;
    result->type = STMT_BLOCK;
    result->statements= statements; 
    return (Statement*)result;
}

Statement* create_if_stmt(Expr* condition, Statement* then, ElifStmt* elif, Statement* elseBranch){
    IfStmt* result = malloc(sizeof(IfStmt));
    if(result == NULL) return NULL;
    result->type = STMT_IF;
    result->condition = condition;
    result->then = then;
    result->elif = elif;
    result->elseBranch = elseBranch;
    return (Statement*)result;
}

ElifStmt* create_elif_stmt(Expr* condition, Statement* then){
    ElifStmt* result = malloc(sizeof(ElifStmt));
    if(result == NULL) return NULL;
    result->condition = condition;
    result->then = then;
    result->next = NULL;
    return result;
}


Statement* create_while_stmt(Expr* condition, Statement* _while){
    WhileStmt* result = malloc(sizeof(WhileStmt));
    if(result == NULL) return NULL;
    result->type = STMT_WHILE;
    result->condition = condition;
    result->_while = _while;
    return (Statement*)result;
}


Statement* create_func_stmt(String identifier, int parameters, Statement* body){
    FunctionStmt* result = malloc(sizeof(FunctionStmt));
    if(result == NULL) return NULL;
    result->type = STMT_FUNC;
    result->identifier = identifier;
    result->parameters = parameters;
    result->body = body;
    return (Statement*)result;
}



Statement* create_return_stmt(Expr* value){
    ReturnStmt* result = malloc(sizeof(ReturnStmt));
    if(result == NULL) return NULL;
    result->type = STMT_RETURN;
    result->value = value; 
    return (Statement*)result;
}

Statement* create_pass_stmt(){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = STMT_PASS;
    result = NULL; 
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
    ElifStmt* elif = NULL;
    parser_try_consume_token(p, TOK_NEW_LINE);

    //make sure the else clause is properly aligned with the top level else 
    //need to do some back tracking to determine proper indentation 
    struct ParserState pstate;
    parser_save_state(p, &pstate);
    while(parser_match_indentation_level(p, expected_indent - 1)){
        if(match(p, TOK_ELIF)){ 
            Expr* elif_condition = expression(p);
            parser_expect_consume_token(p, TOK_COLON);
            parser_expect_consume_token(p, TOK_NEW_LINE);
            Statement* elif_then = block_statement(p, expected_indent);
            ElifStmt* elif_stmt = create_elif_stmt(elif_condition, elif_then);
            if(elif == NULL){
                elif = elif_stmt;
            } else{
                ElifStmt* curr = elif;
                while(curr->next != NULL) curr = (ElifStmt*)curr->next;
                curr->next = (struct ElifStmt*)elif_stmt;
            }
            parser_try_consume_token(p, TOK_NEW_LINE);
        } else if(match(p, TOK_ELSE)){
            parser_expect_consume_token(p, TOK_COLON);
            parser_expect_consume_token(p, TOK_NEW_LINE);
            elseBranch = block_statement(p, expected_indent);
            parser_try_consume_token(p, TOK_NEW_LINE);
            break;
        } else{
            parser_restore_state(p, &pstate);
            break;
        }
        parser_save_state(p, &pstate);
    }
    return create_if_stmt(condition, then, elif, elseBranch);
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
    int parameters = 0;

    //TODO: VARADIC AND KEYWORD ARGS 
    if(p->currentToken.type != TOK_RIGHT_PAREN){
       do {
           if(parameters >= 10){
                parser_new_error(p, "Invalid Parameter count: %s. Functions only support 10 parameters\n", identifier.str);
           }
            
           parser_expect_token(p, TOK_IDENTIFIER);
           parameters++;
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


void delete_statement(Statement* statement){ 
    if(statement == NULL) return;
    switch (statement->type) {
        case STMT_ASSIGN: {
            AssignStmt* s = (AssignStmt*)(statement);
            delete_expr_tree(s->value);
            string_delete(&s->identifier);
            free(s);
            s = NULL;
            break;
        }
        case STMT_ASSIGN_OP: {
            AssignOpStmt* s = (AssignOpStmt*)(statement);
            delete_expr_tree(s->value);
            string_delete(&s->identifier);
            free(s);
            break;
        }
        case STMT_EXPR: {
            ExprStmt* s = (ExprStmt*)(statement);
            delete_expr_tree(s->expr);
            free(s);
            break;
        }
        case STMT_BLOCK: {
            BlockStmt* s = (BlockStmt*)(statement);
            for(int i =0; i < s->statements.size; i++){
                Statement* temp = array_list_get(s->statements, Statement*, i);
                delete_statement(temp);
            }
            array_list_delete(s->statements);
            free(s);
            break;
        }
        case STMT_IF: {
            IfStmt* s = (IfStmt*)(statement);
            delete_expr_tree(s->condition);
            delete_statement(s->then);
            if(s->elseBranch != NULL){
                delete_statement(s->elseBranch);
            }
            ElifStmt* eifs = s->elif;
            while(eifs != NULL){
                ElifStmt* temp = (ElifStmt*)eifs->next;
                delete_expr_tree(eifs->condition);
                delete_statement(eifs->then);
                free(eifs);
                eifs = temp;
            }
            free(s);
            break;
        }
        case STMT_WHILE: {
            WhileStmt* s = (WhileStmt*)(statement);
            delete_expr_tree(s->condition);
            delete_statement(s->_while);
            free(s);
            break;
        }
        case STMT_FUNC: {
            FunctionStmt* s = (FunctionStmt*)(statement);
            delete_statement(s->body);
            string_delete(&s->identifier);
            free(s);
            break;
        } 
        case STMT_RETURN: {
            ReturnStmt* s = (ReturnStmt*)(statement); 
            free(s);
            break;
        }
        case STMT_PASS:
            break;
        default:
            return; 
    }

}

