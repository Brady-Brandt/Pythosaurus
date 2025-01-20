#include "statement.h"
#include "arraylist.h"
#include "parser.h"
#include "expression.h"
#include "print.h"
#include "stringtype.h"
#include "tokenizer.h"


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


static Statement* create_assign_stmt(uint32_t line, String* id, Expr* value){
    AssignStmt* result = malloc(sizeof(AssignStmt));
    if(result == NULL) return NULL;
    result->type = STMT_ASSIGN;
    result->line = line;
    result->identifier = id;
    result->value = value; 
    return (Statement*)result;
}


static Statement* create_op_assign_stmt(uint32_t line, String* id, TokenType operator, Expr* value){
    AssignOpStmt* result = malloc(sizeof(AssignOpStmt));
    if(result == NULL) return NULL;
    result->type = STMT_ASSIGN_OP;
    result->identifier = id;
    result->line = line;
    result->value = value;
    result->op = operator;
    return (Statement*)result;
}


static Statement* create_expr_stmt(uint32_t line, Expr* expr){
    ExprStmt* result = malloc(sizeof(ExprStmt));
    if(result == NULL) return NULL;
    result->type = STMT_EXPR;
    result->line = line;
    result->expr = expr; 
    return (Statement*)result;
}


static Statement* create_block_stmt(uint32_t line, ArrayList* statements){
    BlockStmt* result = malloc(sizeof(BlockStmt));
    if(result == NULL) return NULL;
    result->type = STMT_BLOCK;
    //the first line of the block 
    result->line = line;
    result->statements= statements; 
    return (Statement*)result;
}

static Statement* create_if_stmt(uint32_t line, Expr* condition, Statement* then, ElifStmt* elif, Statement* elseBranch){
    IfStmt* result = malloc(sizeof(IfStmt));
    if(result == NULL) return NULL;
    result->type = STMT_IF;
    result->condition = condition;
    result->line = line;
    result->then = then;
    result->elif = elif;
    result->elseBranch = elseBranch;
    return (Statement*)result;
}

static ElifStmt* create_elif_stmt(uint32_t line, Expr* condition, Statement* then){
    ElifStmt* result = malloc(sizeof(ElifStmt));
    if(result == NULL) return NULL;
    result->condition = condition;
    result->line = line;
    result->then = then;
    result->next = NULL;
    return result;
}


static Statement* create_while_stmt(uint32_t line, Expr* condition, Statement* _while){
    WhileStmt* result = malloc(sizeof(WhileStmt));
    if(result == NULL) return NULL;
    result->type = STMT_WHILE;
    result->line = line;
    result->condition = condition;
    result->_while = _while;
    return (Statement*)result;
}


static Statement* create_func_stmt(uint32_t line, String* identifier, ArrayList* parameters, Statement* body, uint32_t var_count){
    FunctionStmt* result = malloc(sizeof(FunctionStmt));
    if(result == NULL) return NULL;
    result->type = STMT_FUNC;
    result->line = line;
    result->identifier = identifier;
    result->parameters = parameters;
    result->body = body;
    result->varCount = var_count;
    return (Statement*)result;
}

static Statement* create_assert_stmt(uint32_t line, Expr* condition, String* msg){
    AssertStmt* result = malloc(sizeof(AssertStmt));
    if(result == NULL) return NULL;
    result->type = STMT_ASSERT;
    result->line = line;
    result->condition = condition;
    result->msg = msg;
    return (Statement*)result;
}

static Statement* create_return_stmt(uint32_t line, Expr* value){
    ReturnStmt* result = malloc(sizeof(ReturnStmt));
    if(result == NULL) return NULL;
    result->line = line;
    result->type = STMT_RETURN;
    result->value = value; 
    return (Statement*)result;
}

static Statement* create_stmt(uint32_t line, StatementType type){
    Statement* result = malloc(sizeof(Statement));
    if(result == NULL) return NULL;
    result->type = type;
    result->line = line;
    result->stmt = NULL; 
    return result;
}

static Statement* create_class_statement(uint32_t line, String* name, ArrayList* super, Statement* body){
    ClassStmt* result = malloc(sizeof(ClassStmt));
    if(result == NULL) return NULL;
    result->line = line;
    result->type = STMT_CLASS;
    result->name = name;
    result->superClasses = super;
    result->body = body;
    return (Statement*)result;
}




static Statement* assign_statement(Parser* p){
    switch (parser_peek_token(p).type) {
        case TOK_ASSIGN: {
            String* identifier = p->currentToken.literal;
            uint32_t line = p->currentToken.line;
            parser_consume_verified_token(p, TOK_IDENTIFIER);
            parser_consume_verified_token(p, TOK_ASSIGN);
            Expr* value = expression(p);
            Statement* stmt = create_assign_stmt(line, identifier, value); //TODO: ADD SUPORT FOR SEMICOLONS TO TERMINATE 
            //STATEMENTS
            parser_expect_token(p, TOK_NEW_LINE);
            parser_inc_var(p);
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
            String* identifier = p->currentToken.literal; 
            uint32_t line = p->currentToken.line;
            Token operation = parser_consume_verified_token(p, TOK_IDENTIFIER);
            //skip over the op
            parser_next_token(p);
            Expr* value = expression(p);
            Statement* stmt = create_op_assign_stmt(line, identifier, operation.type, value);
            parser_expect_token(p, TOK_NEW_LINE);
            return stmt; 
        }
        default:{
            uint32_t line = p->currentToken.line;
            Expr* value = expression(p);
            parser_expect_token(p, TOK_NEW_LINE);
            return create_expr_stmt(line, value);
        } 
    }      
}



static Statement* block_statement(Parser* p, unsigned int expected_indent){
    p->indentationLevel++;
    ArrayList* statements;
    array_list_create_cap(statements, Statement*, 10);
    uint32_t line = p->currentToken.line;
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
    return create_block_stmt(line, statements);
}



static Statement* if_statement(Parser *p){
    unsigned int expected_indent = p->indentationLevel + 1;
    uint32_t if_line = p->currentToken.line;
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
            uint32_t elif_line = p->currentToken.line;
            parser_expect_consume_token(p, TOK_COLON);
            parser_expect_consume_token(p, TOK_NEW_LINE);
            Statement* elif_then = block_statement(p, expected_indent);
            ElifStmt* elif_stmt = create_elif_stmt(elif_line, elif_condition, elif_then);
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
    return create_if_stmt(if_line, condition, then, elif, elseBranch);
}


static Statement* while_statement(Parser *p){
    unsigned int expected_indent = p->indentationLevel + 1;
    uint32_t line = p->currentToken.line;
    parser_consume_verified_token(p, TOK_WHILE);
    Expr* cond = expression(p);
    parser_expect_consume_token(p, TOK_COLON);
    parser_expect_consume_token(p, TOK_NEW_LINE);
    Statement* _while = block_statement(p, expected_indent);
    return create_while_stmt(line, cond, _while);
}



static Statement* function_statement(Parser *p){
    uint32_t line = p->currentToken.line;
    parser_consume_verified_token(p, TOK_DEF);
    parser_expect_consume_token(p, TOK_IDENTIFIER);
    String* identifier = parser_prev_token(p).literal;
    parser_expect_consume_token(p, TOK_LEFT_PAREN);
    ArrayList* params;
    array_list_create_cap(params, String, 10);

    //TODO: VARADIC AND KEYWORD ARGS 
    if(p->currentToken.type != TOK_RIGHT_PAREN){
       do {
           if(params->size >= 10){
                parser_new_error(p, "Invalid Parameter count: %s. Functions only support 10 parameters\n", get_str(identifier));
           } 
           parser_expect_token(p, TOK_IDENTIFIER);
           array_list_append(params, String*, p->currentToken.literal);
           parser_next_token(p);
       }while (match(p, TOK_COMMA)); 
    }

    parser_expect_consume_token(p, TOK_RIGHT_PAREN);
    parser_expect_consume_token(p, TOK_COLON);
    parser_expect_consume_token(p, TOK_NEW_LINE);
    unsigned int expected_indent = p->indentationLevel + 1;
    Statement* body = block_statement(p, expected_indent);
    parser_inc_func(p);
    uint32_t var_count = p->funcVarc + array_list_size(params);
    //remove the local function counts from the global counts
    p->varDefc -= p->funcVarc;

    return create_func_stmt(line, identifier, params, body, var_count);
}


static Statement* assert_statement(Parser *p){
    uint32_t line = p->currentToken.line;
    parser_consume_verified_token(p, TOK_ASSERT);
    Expr* condition = expression(p);
    String* msg = NULL;
    if(p->currentToken.type == TOK_COMMA){ 
        parser_consume_verified_token(p, TOK_COMMA);
        parser_expect_consume_token(p, TOK_STRING);
        msg = parser_prev_token(p).literal;
    }
    parser_expect_consume_token(p, TOK_NEW_LINE);
    return create_assert_stmt(line, condition, msg);
}



static Statement* return_statement(Parser *p){
    uint32_t line = p->currentToken.line;
    parser_consume_verified_token(p, TOK_RETURN);
    Expr* value = (Expr*)None_Expr;
    if(p->currentToken.type != TOK_NEW_LINE){
        value = expression(p); 
    }
    return create_return_stmt(line, value);
}


static Statement* pass_statement(Parser *p){
    uint32_t line = p->currentToken.line;
    parser_consume_verified_token(p, TOK_PASS);
    parser_expect_token(p, TOK_NEW_LINE);
    return create_stmt(line, STMT_PASS);
}

static Statement* break_statement(Parser *p){
    uint32_t line = p->currentToken.line;
    parser_consume_verified_token(p, TOK_BREAK);
    parser_expect_token(p, TOK_NEW_LINE);
    return create_stmt(line, STMT_BREAK);
}

static Statement* continue_statement(Parser *p){
    uint32_t line = p->currentToken.line;
    parser_consume_verified_token(p, TOK_CONTINUE);
    parser_expect_token(p, TOK_NEW_LINE);
    return create_stmt(line, STMT_CONTINUE);
}


//combine these two statements because parsing them is almost the exact same 
static Statement* global_del_statement(Parser *p, StatementType stype){
    uint32_t line = p->currentToken.line;

    bool is_global = true;
    if(p->currentToken.type == TOK_GLOBAL){
        is_global = true;
    }

    parser_next_token(p); //consume either the Global || Del keyword
    ArrayList* vars;
    array_list_create_cap(vars, Expr*, 2);
    bool inside_paren = false;
    if(p->currentToken.type == TOK_LEFT_PAREN){
        inside_paren = true;
        parser_next_token(p);
    }
    do {
        Expr* variable = expression(p);
        array_list_append(vars, Expr*, variable);
    }while (match(p, TOK_COMMA)); 

    if(inside_paren) parser_expect_consume_token(p, TOK_RIGHT_PAREN);

    if(vars->size == 0){
        parser_new_error(p, "Expected at least one value");
    }

    if(is_global){
       p->funcVarc += vars->size;
       //when calculating a the number of variables in global 
       //scope, when take p->varDefc - p->funcVarc  
       //to preserve the amount in global scope we need vars->size * 2
       p->varDefc += vars->size * 2;
    }

    parser_consume_token(p, TOK_NEW_LINE, "Missing new line");
 
    GlobalDelStmt* result = malloc(sizeof(GlobalDelStmt));
    if(result == NULL) return NULL;
    result->line = line;
    result->type = stype;
    result->values = vars;
    return (Statement*)result;
}



static Statement* class_statement(Parser *p){
    unsigned int expected_indent = p->indentationLevel + 1;
    uint32_t line = p->currentToken.line;
    parser_consume_verified_token(p, TOK_CLASS);
    parser_expect_consume_token(p, TOK_IDENTIFIER);
    String* name = parser_prev_token(p).literal;
    ArrayList* super = NULL;

    //gets the superclasses if there are any
    if(match(p, TOK_LEFT_PAREN)){
        array_list_create_cap(super, String, 1);
        do {  
           parser_expect_token(p, TOK_IDENTIFIER);
           array_list_append(super, String*, p->currentToken.literal);
           parser_next_token(p);
        }while (match(p, TOK_COMMA));
    }

    parser_expect_consume_token(p, TOK_RIGHT_PAREN);
    parser_expect_consume_token(p, TOK_COLON);
    parser_expect_consume_token(p, TOK_NEW_LINE);
    Statement* body = block_statement(p, expected_indent);
    return create_class_statement(line, name, super, body);
}


Statement* statement(Parser *p){
    parser_try_consume_token(p, TOK_NEW_LINE); 
    switch(p->currentToken.type){
        case TOK_IDENTIFIER:
            return assign_statement(p);
        case TOK_IF:
            return if_statement(p);
        case TOK_WHILE:
            return while_statement(p);
        case TOK_DEF:
            return function_statement(p);
        case TOK_ASSERT:
            return assert_statement(p);
        case TOK_RETURN:
            return return_statement(p);
        case TOK_PASS:
            return pass_statement(p);
        case TOK_BREAK:
            return break_statement(p);
        case TOK_CONTINUE:
            return continue_statement(p);
        case TOK_DEL:
            return global_del_statement(p, STMT_DEL);
        case TOK_GLOBAL:
            return global_del_statement(p, STMT_GLOBAL);
        case TOK_CLASS:
            return class_statement(p);
        case TOK_EOF:
            return NULL;
        default:
            parser_new_error(p, "Not Implemented error %s", get_token_type(p->currentToken.type));
            return NULL;
    } 
}


void delete_statement(Statement* statement){ 
    if(statement == NULL) return;
    switch (statement->type) {
        case STMT_ASSIGN: {
            AssignStmt* s = (AssignStmt*)(statement);
            delete_expr_tree(s->value);
            string_delete(s->identifier);
            free(s);
            s = NULL;
            break;
        }
        case STMT_ASSIGN_OP: {
            AssignOpStmt* s = (AssignOpStmt*)(statement);
            delete_expr_tree(s->value);
            string_delete(s->identifier);
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
            for(int i =0; i < array_list_size(s->statements); i++){
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
        case STMT_CLASS: {
            ClassStmt* s = (ClassStmt*)(statement);
            for(int i = 0; i < array_list_size(s->superClasses); i++){

            }
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
            string_delete(s->identifier); 
            array_list_delete(s->parameters);
            free(s);
            break;
        } 
        case STMT_RETURN: {
            ReturnStmt* s = (ReturnStmt*)(statement); 
            delete_expr_tree(s->value);
            free(s);
            break;
        }
        case STMT_ASSERT: {
            AssertStmt* s = (AssertStmt*)(statement);
            delete_expr_tree(s->condition);
            string_delete(s->msg);
            free(s);
            break;
        }
        case STMT_PASS:
        case STMT_BREAK:
        case STMT_CONTINUE:
            free(statement);
            break;
        case STMT_GLOBAL:
        case STMT_DEL: {
            GlobalDelStmt* s = (GlobalDelStmt*)statement;
            for(int i = 0; i < array_list_size(s->values); i++){
                Expr* expr = array_list_get(s->values, Expr*, i);
                delete_expr_tree(expr);
            }
            free(s);
            break;
        }
        default:
            return; 
    }
}

