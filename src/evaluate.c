#include "evaluate.h"
#include "allocator.h"
#include "arraylist.h"
#include "interpret.h"
#include "expression.h"
#include "type_util.h"
#include "print.h"
#include "type_op.h"
#include "statement.h"
#include "stringtype.h"
#include "tokenizer.h"


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static LiteralExpr* evaluate_unary_expression(Interpretor *interpret,Expr* expr){
   UnaryExpr* uexpr = (UnaryExpr*)expr;
   LiteralExpr* right = evaluate_expression(interpret, uexpr->right);

   LiteralExpr result;
   switch (uexpr->op) {
        case TOK_BIT_NOT:
            bit_not(interpret, right, &result); 
            break;
        case TOK_SUB:
            usub(interpret, right, &result); 
            break;
        case TOK_ADD:
            uadd(interpret, right, &result); 
            break;
        case TOK_NOT:
            not(interpret, right, &result); 
            break;
        default:
            interpretor_throw_error(interpret, "Invalid unary operator %s\n", get_token_type(uexpr->op));
   }
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator);
}



static LiteralExpr* evaluate_binary_expression(Interpretor *interpret,Expr* expr) {
    BinaryExpr* bexpr = (BinaryExpr*)expr;
    LiteralExpr* left = evaluate_expression(interpret, bexpr->left); 

    //if the left side of an and expression is false 
    //the whole expression is false
    if(bexpr->op == TOK_AND){
        if(as_bool(interpret, left) == false){
            LiteralExpr result;
            result.litType = LIT_BOOL;
            result.boolean = false;
            allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
            return allocator_peek(&interpret->expressionAllocator);
        }
    } else if (bexpr->op == TOK_OR) {
        //return true if the left side is true 
        if(as_bool(interpret, left)){
            LiteralExpr result;
            result.litType = LIT_BOOL;
            result.boolean = true;
            allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
            return allocator_peek(&interpret->expressionAllocator);
        }
    
    }

    LiteralExpr* right = evaluate_expression(interpret, bexpr->right); 
    LiteralExpr result; 
    switch (bexpr->op) {
        case TOK_ADD:
            add(interpret, left, right, &result);
            break;
        case TOK_SUB:
            sub(interpret, left, right, &result);
            break;
        case TOK_MUL:
            mul(interpret, left, right, &result);
            break;
        case TOK_DIV:
            divide(interpret, left, right, &result);
            break;
        case TOK_MOD:
            mod(interpret, left, right, &result);
            break;
        case TOK_FLOOR_DIV:
            floor_div(interpret, left, right, &result);
            break;
        case TOK_EXP:
            expo(interpret, left, right, &result);
            break;
        case TOK_AND:
            and(interpret, left, right, &result);
            break;
        case TOK_OR:
            or(interpret, left, right, &result);
            break;
        case TOK_LESS_THAN:
            less_than(interpret, left, right, &result);
            break;
        case TOK_GREATER_THAN:
            greater_than(interpret, left, right, &result);
            break;
        case TOK_LESS_EQUAL:
            less_equal(interpret, left, right, &result);
            break;
        case TOK_GREATER_EQUAL:
            greater_equal(interpret, left, right, &result);
            break;
        case TOK_EQUAL:
            equal(interpret, left, right, &result);
            break;
        case TOK_NOT_EQUAL:
            equal(interpret, left, right, &result);
            result.boolean = !result.boolean;
            break;
        case TOK_BIT_AND:
            bit_and(interpret, left, right, &result);
            break;
        case TOK_BIT_OR:
            bit_or(interpret, left, right, &result);
            break;
        case TOK_BIT_XOR:
            bit_xor(interpret, left, right, &result);
            break;
        case TOK_LEFT_SHIFT:
            left_shift(interpret, left, right, &result);
            break;
        case TOK_RIGHT_SHIFT:
            right_shift(interpret, left, right, &result);
            break;
        default:
            interpretor_throw_error(interpret, "Invalid binary operator %s\n", get_token_type(bexpr->op));
    }
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator);
}

static LiteralExpr* evaluate_assignop_stmt(Interpretor *interpret, LiteralExpr* variable, TokenType op, Expr* val){
    LiteralExpr* right = evaluate_expression(interpret, val);
    LiteralExpr result;
    switch (op) {
        case TOK_ADD_ASSIGN:
            add(interpret, variable, right, &result);
            break;
        case TOK_SUB_ASSIGN:
            sub(interpret, variable, right, &result);
            break;
        case TOK_MUL_ASSIGN:
            mul(interpret, variable, right, &result);
            break;
        case TOK_DIV_ASSIGN:
            divide(interpret, variable, right, &result);
            break;
        case TOK_MOD_ASSIGN:
            mod(interpret, variable, right, &result);
            break;
        case TOK_FLOOR_DIV_ASSIGN:
            floor_div(interpret, variable, right, &result);
            break;
        case TOK_EXP_ASSIGN:
            expo(interpret, variable, right, &result);
            break;
        case TOK_BIT_AND_ASSIGN:
            bit_and(interpret, variable, right, &result);
            break;
        case TOK_BIT_OR_ASSIGN:
            bit_or(interpret, variable, right, &result);
            break;
        case TOK_BIT_XOR_ASSIGN:
            bit_xor(interpret, variable, right, &result);
            break;
        case TOK_LEFT_SHIFT_ASSIGN:
            left_shift(interpret, variable, right, &result);
            break;
        case TOK_RIGHT_SHIFT_ASSIGN:
            right_shift(interpret, variable, right, &result);
            break;
        default:
            interpretor_throw_error(interpret, "Invalid assignment operator %s\n", get_token_type(op));
    }
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator);
}



static LiteralExpr* evaluate_function_expression(Interpretor *interpret,Expr* expr){
    FuncExpr* fexpr = (FuncExpr*)expr;
    FuncArgs args = {0};

    for(int i = 0; i < fexpr->args.size; i++){
        Expr* param = array_list_get(fexpr->args, Expr*, i);
        LiteralExpr* evaluatedParam = evaluate_expression(interpret, param);
        func_args_add(&args, (struct LiteralExpr*)evaluatedParam);
    }
   
    return (LiteralExpr*)interpretor_call_function(interpret, fexpr->name, args);
}

LiteralExpr* evaluate_expression(Interpretor *interpret, Expr* expression){
    if(expression == NULL){
        interpretor_throw_error(interpret, "Invalid Expression detected");
    }

    switch (expression->type) {
        case EXPR_BINARY:
            return evaluate_binary_expression(interpret, expression);
        case EXPR_LITERAL:{
            LiteralExpr* lexpr = (LiteralExpr*)expression;

            //if it is a variable, we get a copy of its value 
            if(lexpr->litType == LIT_IDENTIFIER){
                LiteralExpr result;
                interpretor_get_var(interpret, lexpr->identifier, (struct LiteralExpr*)&result); 
                allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
                return (LiteralExpr*)allocator_peek(&interpret->expressionAllocator);
            }
            return lexpr; 
        }
        case EXPR_UNARY:
            return evaluate_unary_expression(interpret,expression);
        case EXPR_GROUPING:
            return evaluate_expression(interpret,expression->expr);
        case EXPR_FUNC: 
            return evaluate_function_expression(interpret,expression);
        default:
            interpretor_throw_error(interpret, "Invalid Expression detected");
    } 
}

void evaluate_statement(Interpretor *interpret, Statement* statement){
    switch (statement->type) {
        case STMT_EXPR:{ 
            ExprStmt* estmt = (ExprStmt*)(statement);
            long start = interpretor_save_expression(interpret);
            evaluate_expression(interpret, estmt->expr);
            interpretor_restore_expression(interpret, start);
            break;
        }
        case STMT_ASSIGN: {
            AssignStmt* astmt = (AssignStmt*)(statement);
            //AS OF RIGHT NOW VARIABLES ARE NOT LAZLILY ASSIGNED
            long start = interpretor_save_expression(interpret);
            LiteralExpr* value = evaluate_expression(interpret, astmt->value);
            interpretor_assign_var((Interpretor*)interpret, astmt->identifier, (struct LiteralExpr*)value);
            interpretor_restore_expression(interpret, start);
            break;
        }
        case STMT_ASSIGN_OP: {
            AssignOpStmt* opstmt = (AssignOpStmt*)(statement);
            LiteralExpr var;
            long start = interpretor_save_expression(interpret);
            interpretor_get_var(interpret, opstmt->identifier, (struct LiteralExpr*)&var);
            LiteralExpr* value = evaluate_assignop_stmt(interpret, &var, opstmt->op, opstmt->value);
            interpretor_assign_var((Interpretor*)interpret, opstmt->identifier, (struct LiteralExpr*)value);
            interpretor_restore_expression(interpret, start);
            break;
        }
        case STMT_WHILE:{
            WhileStmt* wstmt = (WhileStmt*)(statement);
            long start = interpretor_save_expression(interpret);
            while(as_bool(interpret,evaluate_expression(interpret, wstmt->condition))){
                interpretor_restore_expression(interpret, start);
                evaluate_statement(interpret, wstmt->_while);
            }
            interpretor_restore_expression(interpret, start);
            break;
        }
        case STMT_IF: {
            IfStmt* ifstmt = (IfStmt*)statement;
            long if_start = interpretor_save_expression(interpret);
            if(as_bool(interpret, evaluate_expression(interpret, ifstmt->condition))){
                interpretor_restore_expression(interpret, if_start);
                evaluate_statement(interpret, ifstmt->then);
            } else {
                //loop through all the elif statements if there are any and evaluate them 
                bool active_elif = false;
                ElifStmt* curr_elif = ifstmt->elif;
                while(curr_elif != NULL){
                    long elif_start = interpretor_save_expression(interpret);
                    if(as_bool(interpret, evaluate_expression(interpret, curr_elif->condition))){
                        interpretor_restore_expression(interpret, elif_start);
                        evaluate_statement(interpret, curr_elif->then);
                        active_elif = true;
                        break;
                    }
                    interpretor_restore_expression(interpret, elif_start);
                    curr_elif = (ElifStmt*)curr_elif->next;
                }
                if(!active_elif && ifstmt->elseBranch != NULL){
                    evaluate_statement(interpret, ifstmt->elseBranch);
                }             
                interpretor_restore_expression(interpret, if_start);
            }
            break;
        }
        case STMT_BLOCK: {
            BlockStmt* bstmt = (BlockStmt*)(statement);
            for(int i = 0; i < bstmt->statements.size; i++){
                Statement* current_statement = array_list_get(bstmt->statements, Statement*, i);
                interpret->currentStmt = current_statement;
                evaluate_statement(interpret, current_statement);
            }
            break;
        }
        case STMT_ASSERT: {
            AssertStmt* astmt = (AssertStmt*)(statement);
            long assert_start = interpretor_save_expression(interpret);
            LiteralExpr* res = evaluate_expression(interpret, astmt->condition);
            if(as_bool(interpret, res) == false){
                //don't have exceptions implemented yet, so we will just throw an error
                interpretor_throw_error(interpret, "AssertionError: %s", (astmt->msg == NULL) ? "" : astmt->msg->str);
            }
            interpretor_restore_expression(interpret, assert_start);
            break;
        }
        case STMT_FUNC:{
            FunctionStmt* fstmt = (FunctionStmt*)(statement);
            interpretor_create_function(interpret, fstmt);
            break;
        }
        case STMT_RETURN: {
            ReturnStmt* rstmt = (ReturnStmt*)(statement);
            LiteralExpr* res = evaluate_expression(interpret, rstmt->value);
            interpretor_return(interpret, res);
            break;
        }
        default:
            printf("Not implemented\n");
    }
}




