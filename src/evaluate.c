#include "evaluate.h"
#include "arraylist.h"
#include "interpret.h"
#include "expression.h"
#include "object.h"
#include "print.h"
#include "statement.h"
#include "stringtype.h"
#include "tokenizer.h"


#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void literal_to_class(LiteralExpr* expr){
    switch (expr->litType) {
        case LIT_INTEGER:
            new_integer(expr->integer);
            break;
        case LIT_FLOAT:
            new_float(expr->_float);
            break;
        case LIT_BOOL:
             new_bool(expr->boolean);
            break;
        case LIT_STRING:
             new_str(expr->string);
            break;
        case LIT_NONE:
            interpretor_stack_push(None);
        default:
            interpretor_throw_error("Not implemented yet"); 
    }
}


static void perform_binary_operation(const char* method){
    Args args;
    args.count = 2;

    call_native_method(method, &args);
    ClassInstance* res = interpretor_stack_peek();
    if(res == NotImplemented){
        //pop not implemented off stack and try the method again 
        interpretor_stack_pop();
        //swap a & b to see if b has a method defined 
        interpretor_stack_swap_two();
        call_native_method(method, &args);
        res = interpretor_stack_peek();
        if(res == NotImplemented){
            interpretor_throw_error("Method not %s not defined");
        }
    }
}

static void perform_unary_operation(const char* method){
    Args args;
    args.count = 1;
    call_native_method(method, &args);
    ClassInstance* res = interpretor_stack_peek();
    if(res == NotImplemented){
        interpretor_throw_error("Method not %s not defined");
    } 
}


static bool fast_as_bool(ClassInstance* s){
    if(is_none_class(s)) return false;

    if(is_number_class(s)){
        long* res = get_primitive(s);
        return *res;
    }

    if(is_str_class(s)){
        String* st = get_primitive(s);
        return st != NULL && str_size(st);
    }
    return true;
}


static void perform_inplace_operation(const char* imethod, const char* reg, String* var){ 
    Class* type = interpretor_stack_get_class_type(2);
    //first result is going to attempt to perform i dunder methods ie iadd,isub 
    if(type->isMutable){
        Args args;
        args.count = 2;
        call_native_method(imethod, &args);
        ClassInstance* res = interpretor_stack_peek();
        if(res != NotImplemented) return;
        //pop NotImplemented off top of the stack 
        interpretor_stack_pop();
    }
    perform_binary_operation(reg);
    ClassInstance* res = interpretor_stack_pop();
    interpretor_assign_var(var, res);
}



static void evaluate_unary_expression(Expr* expr){
   UnaryExpr* uexpr = (UnaryExpr*)expr;
   evaluate_expression(uexpr->right);
   switch (uexpr->op) {
        case TOK_BIT_NOT:
            perform_unary_operation(__INVERT__);
            break;
        case TOK_SUB:
            perform_unary_operation(__NEG__);
            break;
        case TOK_ADD:
            perform_unary_operation(__POS__);
            break;
        case TOK_NOT: {
            perform_unary_operation(__BOOL__);
            ClassInstance* b = interpretor_stack_peek();
            b->pbool = !b->pbool;
            break;
        }
        default:
            interpretor_throw_error("Invalid unary operator %s\n", get_token_type(uexpr->op));
   }
}



static void evaluate_binary_expression(Expr* expr) {
    BinaryExpr* bexpr = (BinaryExpr*)expr;
    evaluate_expression(bexpr->left); 
    //if the left side of an and expression is false 
    //the whole expression is false
    if(bexpr->op == TOK_AND){
        perform_unary_operation(__BOOL__);
        ClassInstance* b = interpretor_stack_peek();
        if(!b->pbool){
            return;
        }
        //if it is true we pop it off because we don't care about it 
        // a and b 
        // if a is true, then the result will depend on b 
        interpretor_stack_pop(); 
    } else if (bexpr->op == TOK_OR) {
        perform_unary_operation(__BOOL__);
        ClassInstance* b = interpretor_stack_peek();
        if(b->pbool){
            return;
        } 
        interpretor_stack_pop(); 
    }

    evaluate_expression(bexpr->right); 
    switch (bexpr->op) {
        case TOK_ADD:
            perform_binary_operation(__ADD__);
            break;
        case TOK_SUB:
            perform_binary_operation(__SUB__);
            break;
        case TOK_MUL:
            perform_binary_operation(__MUL__);
            break;
        case TOK_DIV:
            perform_binary_operation(__TRUEDIV__);
            break;
        case TOK_MOD:
            perform_binary_operation(__MOD__);
            break;
        case TOK_FLOOR_DIV:
            perform_binary_operation(__FLOORDIV__);
            break;
        case TOK_EXP:
            perform_binary_operation(__POW__);
            break;
        case TOK_AND: 
        case TOK_OR:
            perform_unary_operation(__BOOL__);
            break;
        case TOK_LESS_THAN:
            perform_binary_operation(__LT__);
            break;
        case TOK_GREATER_THAN:
            perform_binary_operation(__GT__);
            break;
        case TOK_LESS_EQUAL:
            perform_binary_operation(__LE__);
            break;
        case TOK_GREATER_EQUAL:
            perform_binary_operation(__GE__);
            break;
        case TOK_EQUAL:
            perform_binary_operation(__EQ__);
            break;
        case TOK_NOT_EQUAL:
            perform_binary_operation(__NE__);
            break;
        case TOK_BIT_AND:
            perform_binary_operation(__AND__);
            break;
        case TOK_BIT_OR:
            perform_binary_operation(__OR__);
            break;
        case TOK_BIT_XOR:
            perform_binary_operation(__XOR__);
            break;
        case TOK_LEFT_SHIFT:
            perform_binary_operation(__LSHIFT__);
            break;
        case TOK_RIGHT_SHIFT:
            perform_binary_operation(__RSHIFT__);
            break;
        default:
            interpretor_throw_error("Invalid binary operator %s\n", get_token_type(bexpr->op));
    }
}

static void evaluate_assignop_stmt(String* var, TokenType op, Expr* val){
    interpretor_push_var(var);
    evaluate_expression(val);
    switch (op) {
        case TOK_ADD_ASSIGN:
            perform_inplace_operation(__IADD__, __ADD__, var);
			break;
        case TOK_SUB_ASSIGN:
            perform_inplace_operation(__ISUB__, __SUB__, var);
			break;
        case TOK_MUL_ASSIGN:
            perform_inplace_operation(__IMUL__, __MUL__, var);
			break;
        case TOK_DIV_ASSIGN:
            perform_inplace_operation(__ITRUEDIV__, __TRUEDIV__, var);
			break;
        case TOK_MOD_ASSIGN:
            perform_inplace_operation(__IMOD__, __MOD__, var);
			break;
        case TOK_FLOOR_DIV_ASSIGN:
            perform_inplace_operation(__IFLOORDIV__, __FLOORDIV__, var);
			break;
        case TOK_EXP_ASSIGN:
            perform_inplace_operation(__IPOW__, __POW__, var);
			break;
        case TOK_BIT_AND_ASSIGN:
            perform_inplace_operation(__IAND__, __AND__, var);
			break;
        case TOK_BIT_OR_ASSIGN:
            perform_inplace_operation(__IOR__, __OR__, var);
			break;
        case TOK_BIT_XOR_ASSIGN:
            perform_inplace_operation(__IXOR__, __XOR__, var);
			break;
        case TOK_LEFT_SHIFT_ASSIGN:
            perform_inplace_operation(__ILSHIFT__, __LSHIFT__, var);
			break;
        case TOK_RIGHT_SHIFT_ASSIGN:
            perform_inplace_operation(__IRSHIFT__, __ILSHIFT__, var);
			break;
        default:
            interpretor_throw_error("Invalid assignment operator %s\n", get_token_type(op));
    }
}


static void evaluate_function_expression(Expr* expr){
    FuncExpr* fexpr = (FuncExpr*)expr;
    Args args = {0};
    for(int i = 0; i < array_list_size(fexpr->args); i++){
        Expr* param = array_list_get(fexpr->args, Expr*, i);
        args.count++;
        evaluate_expression(param);
    }
    interpretor_call_function(fexpr->name, &args);
}

void evaluate_expression(Expr* expression){
    if(expression == NULL) goto INVALID; 
    switch (expression->type) {
        case EXPR_BINARY:
            evaluate_binary_expression(expression);
            break;
        case EXPR_LITERAL:{
            LiteralExpr* lexpr = (LiteralExpr*)expression;
            //if it is a variable, we get a copy of its value 
            if(lexpr->litType == LIT_IDENTIFIER){
                interpretor_push_var(lexpr->identifier); 
            } else{
                literal_to_class(lexpr); 
            }
            break;
        }
        case EXPR_UNARY:
            evaluate_unary_expression(expression);
            break;
        case EXPR_GROUPING:
            evaluate_expression(expression->expr);
            break;
        case EXPR_FUNC: 
            evaluate_function_expression(expression);
            break;
        default:
        INVALID:
            interpretor_throw_error("Invalid Expression detected");
    } 
}

#define BREAK 1
#define CONTINUE 2

//used to implement break and continue 
static jmp_buf loop_buffer;
static bool in_loop = false;

void evaluate_statement(Statement* statement){
    switch (statement->type) {
        case STMT_EXPR:{ 
            ExprStmt* estmt = (ExprStmt*)(statement);
            evaluate_expression(estmt->expr);
            interpretor_stack_pop(); //remove the garbage the expression put 
            break;
        }
        case STMT_ASSIGN: {
            AssignStmt* astmt = (AssignStmt*)(statement);
            //AS OF RIGHT NOW VARIABLES ARE NOT LAZLILY ASSIGNED
            evaluate_expression(astmt->value);
            ClassInstance* value = interpretor_stack_pop();
            interpretor_assign_var(astmt->identifier, value);
            break;
        }
        case STMT_ASSIGN_OP: {
            AssignOpStmt* opstmt = (AssignOpStmt*)(statement);
            evaluate_assignop_stmt(opstmt->identifier, opstmt->op, opstmt->value);
            break;
        }
        case STMT_WHILE:{
            WhileStmt* wstmt = (WhileStmt*)(statement);
            while(true){
                //check the condition
                evaluate_expression(wstmt->condition);
                ClassInstance* condition = interpretor_stack_pop();
                if(!fast_as_bool(condition)){
                    break;
                }
  
                BlockStmt* while_block = (BlockStmt*)wstmt->_while; 
                CONTINUE_LOOP:
                for(int i = 0; i < array_list_size(while_block->statements); i++){
                    Statement* current_statement = array_list_get(while_block->statements, Statement*, i);
                    interpretor_set_stmt(current_statement);
                    in_loop = true;
                    switch (setjmp(loop_buffer)) {
                        case CONTINUE:
                            goto CONTINUE_LOOP;
                        case BREAK:
                            goto END_LOOP; 
                    }
                    evaluate_statement(current_statement);
                }
            }
            END_LOOP:
            in_loop = false;
            break;
        }
        case STMT_IF: {
            IfStmt* ifstmt = (IfStmt*)statement;
            evaluate_expression(ifstmt->condition);
            ClassInstance* if_condition = interpretor_stack_pop();

            if(fast_as_bool(if_condition)){
                evaluate_statement(ifstmt->then);
            } else {
                //loop through all the elif statements if there are any and evaluate them 
                bool active_elif = false;
                ElifStmt* curr_elif = ifstmt->elif;
                while(curr_elif != NULL){
                    evaluate_expression(curr_elif->condition);
                    ClassInstance* elif_condition = interpretor_stack_pop();
                    if(fast_as_bool(elif_condition)){
                        evaluate_statement(curr_elif->then);
                        active_elif = true;
                        break;
                    }
                    curr_elif = (ElifStmt*)curr_elif->next;
                }
                if(!active_elif && ifstmt->elseBranch != NULL){
                    evaluate_statement(ifstmt->elseBranch);
                }             
            }
            break;
        }
        case STMT_BLOCK: {
            BlockStmt* bstmt = (BlockStmt*)(statement);
            for(int i = 0; i < array_list_size(bstmt->statements); i++){
                Statement* current_statement = array_list_get(bstmt->statements, Statement*, i);
                interpretor_set_stmt(current_statement);
                evaluate_statement(current_statement);
            }
            break;
        }
        case STMT_ASSERT: {
            AssertStmt* astmt = (AssertStmt*)(statement);
            evaluate_expression(astmt->condition);
            ClassInstance* temp = interpretor_stack_pop();
            if(fast_as_bool(temp) == false){
                //don't have exceptions implemented yet, so we will just throw an error
                interpretor_throw_error("AssertionError: %s", (astmt->msg == NULL) ? "" : get_str(astmt->msg));
            }
            break;
        }
        case STMT_FUNC:{
            FunctionStmt* fstmt = (FunctionStmt*)(statement);
            interpretor_create_function(fstmt);
            break;
        }
        case STMT_RETURN: {
            ReturnStmt* rstmt = (ReturnStmt*)(statement);
            //the return value will be at the top of the operand stack
            evaluate_expression(rstmt->value);
            interpretor_return();
            break;
        }
        case STMT_BREAK: {
            if(!in_loop) interpretor_throw_error("Cannot use break outside of loop");
            longjmp(loop_buffer, BREAK);
            break;
        }
        case STMT_CONTINUE: {
            if(!in_loop) interpretor_throw_error("Cannot use continue outside of loop");
            longjmp(loop_buffer, CONTINUE);
            break;
        }
        case STMT_PASS:
            return;
        case STMT_DEL: {
            //TODO: once we add dicts and lists we need to support deleting indices/entries 
            GlobalDelStmt* gdstmt = (GlobalDelStmt*)(statement);
            for(int i = 0; i < array_list_size(gdstmt->values); i++){
                Expr* temp = array_list_get(gdstmt->values, Expr*, i);
                if(temp->type == EXPR_LITERAL && ((LiteralExpr*)temp)->litType == LIT_IDENTIFIER){
                    LiteralExpr* val = (LiteralExpr*)temp;
                    interpretor_del_value(val); 
                } else{
                    interpretor_throw_error("Cannot delete Value");
                }
            } 
            break;
        }
        case STMT_GLOBAL: {
            GlobalDelStmt* gdstmt = (GlobalDelStmt*)(statement);
            for(int i = 0; i < array_list_size(gdstmt->values); i++){
                Expr* temp = array_list_get(gdstmt->values, Expr*, i);
                if(temp->type == EXPR_LITERAL && ((LiteralExpr*)temp)->litType == LIT_IDENTIFIER){
                    LiteralExpr* val = (LiteralExpr*)temp;
                    interpretor_global_var(val->identifier); 
                } else{
                    interpretor_throw_error("Invalid variable");
                }
            }
            break;
        }
        default:
            printf("Not implemented\n");
    }
}




