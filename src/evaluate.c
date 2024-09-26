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


static ClassInstance* Literal_to_class(LiteralExpr* expr){
    switch (expr->litType) {
        case LIT_INTEGER:
            return new_integer(expr->integer);
        case LIT_FLOAT:
            return new_float(expr->_float);
        case LIT_BOOL:
            return new_bool(expr->boolean);
        case LIT_STRING:
            return new_str(expr->string);
        case LIT_NONE:
            return None;
        default:
            interpretor_throw_error("Not implemented yet"); 
    }
}


static ClassInstance* perform_binary_operation(const char* method, ClassInstance* a, ClassInstance* b){
    MethodArgs args;
    args.args[0] = a;
    args.args[1] = b;
    args.count = 2;
    args.self = true;
    ClassInstance* res = call_native_method(a, method, &args);
    if(res == NotImplemented){
        args.args[0] = b;
        args.args[1] = a;
        res = call_native_method(b, method, &args);
        if(res == NotImplemented){
            interpretor_throw_error("Method not %s not defined");
        }
    }
    return res;
}

static ClassInstance* perform_unary_operation(const char* method, ClassInstance* a){
    MethodArgs args;
    args.args[0] = a;
    args.count = 1;
    args.self = true;
    ClassInstance* res = call_native_method(a, method, &args);
    if(res == NotImplemented){
        interpretor_throw_error("Method not %s not defined");
    } 
    return res;
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


static void perform_inplace_operation(const char* imethod, const char* reg, String* var, ClassInstance* b){

    ClassInstance* self = interpretor_get_var(var);

    //first result is going to attempt to perform i dunder methods ie iadd,isub 
    if(self->classType->isMutable){
        MethodArgs args;
        args.args[0] = self;
        args.args[1] = b;
        args.count = 2;
        args.self = true;
        ClassInstance* res = call_native_method(self, imethod, &args);
        if(res != NotImplemented) return;
    }

    ClassInstance* res = perform_binary_operation(reg, self, b);
    interpretor_assign_var(var, res);
}



static ClassInstance* evaluate_unary_expression(Expr* expr){
   UnaryExpr* uexpr = (UnaryExpr*)expr;
   ClassInstance* right = evaluate_expression(uexpr->right);

   switch (uexpr->op) {
        case TOK_BIT_NOT:
            return perform_unary_operation(__INVERT__, right);
        case TOK_SUB:
            return perform_unary_operation(__NEG__, right);
        case TOK_ADD:
            return perform_unary_operation(__POS__, right);
        case TOK_NOT: {
            ClassInstance* b = perform_unary_operation(__BOOL__, right);
            b->pbool = !b->pbool;
            return b;
        }
        default:
            interpretor_throw_error("Invalid unary operator %s\n", get_token_type(uexpr->op));
   }
}



static ClassInstance* evaluate_binary_expression(Expr* expr) {
    BinaryExpr* bexpr = (BinaryExpr*)expr;
    ClassInstance* left = evaluate_expression(bexpr->left); 

    //if the left side of an and expression is false 
    //the whole expression is false
    if(bexpr->op == TOK_AND){
        ClassInstance* b = perform_unary_operation(__BOOL__, left);
        if(!b->pbool){
            return b;
        }
    } else if (bexpr->op == TOK_OR) {
        ClassInstance* b = perform_unary_operation(__BOOL__, left);
        if(b->pbool){
            return b;
        } 
    }

    ClassInstance* right = evaluate_expression(bexpr->right); 
    switch (bexpr->op) {
        case TOK_ADD:
            return perform_binary_operation(__ADD__, left, right);
            break;
        case TOK_SUB:
            return perform_binary_operation(__SUB__, left, right);
            break;
        case TOK_MUL:
            return perform_binary_operation(__MUL__, left, right);
            break;
        case TOK_DIV:
            return perform_binary_operation(__TRUEDIV__, left, right);
            break;
        case TOK_MOD:
            return perform_binary_operation(__MOD__, left, right);
            break;
        case TOK_FLOOR_DIV:
            return perform_binary_operation(__FLOORDIV__, left, right);
            break;
        case TOK_EXP:
            return perform_binary_operation(__POW__, left, right);
            break;
        case TOK_AND: 
        case TOK_OR:
            return perform_unary_operation(__BOOL__, right);
        case TOK_LESS_THAN:
            return perform_binary_operation(__LT__, left, right);
            break;
        case TOK_GREATER_THAN:
            return perform_binary_operation(__GT__, left, right);
            break;
        case TOK_LESS_EQUAL:
            return perform_binary_operation(__LE__, left, right);
            break;
        case TOK_GREATER_EQUAL:
            return perform_binary_operation(__GE__, left, right);
            break;
        case TOK_EQUAL:
            return perform_binary_operation(__EQ__, left, right);
            break;
        case TOK_NOT_EQUAL:
            return perform_binary_operation(__NE__, left, right);
            break;
        case TOK_BIT_AND:
            return perform_binary_operation(__AND__, left, right);
            break;
        case TOK_BIT_OR:
            return perform_binary_operation(__OR__, left, right);
            break;
        case TOK_BIT_XOR:
            return perform_binary_operation(__XOR__, left, right);
            break;
        case TOK_LEFT_SHIFT:
            return perform_binary_operation(__LSHIFT__, left, right);
            break;
        case TOK_RIGHT_SHIFT:
            return perform_binary_operation(__RSHIFT__, left, right);
            break;
        default:
            interpretor_throw_error("Invalid binary operator %s\n", get_token_type(bexpr->op));
    }
}

static void evaluate_assignop_stmt(String* var, TokenType op, Expr* val){
    ClassInstance* right = evaluate_expression(val);
    switch (op) {
        case TOK_ADD_ASSIGN:
            perform_inplace_operation(__IADD__, __ADD__, var, right);
			break;
        case TOK_SUB_ASSIGN:
            perform_inplace_operation(__ISUB__, __SUB__, var, right);
			break;
        case TOK_MUL_ASSIGN:
            perform_inplace_operation(__IMUL__, __MUL__, var, right);
			break;
        case TOK_DIV_ASSIGN:
            perform_inplace_operation(__ITRUEDIV__, __TRUEDIV__, var, right);
			break;
        case TOK_MOD_ASSIGN:
            perform_inplace_operation(__IMOD__, __MOD__, var, right);
			break;
        case TOK_FLOOR_DIV_ASSIGN:
            perform_inplace_operation(__IFLOORDIV__, __FLOORDIV__, var, right);
			break;
        case TOK_EXP_ASSIGN:
            perform_inplace_operation(__IPOW__, __POW__, var, right);
			break;
        case TOK_BIT_AND_ASSIGN:
            perform_inplace_operation(__IAND__, __AND__, var, right);
			break;
        case TOK_BIT_OR_ASSIGN:
            perform_inplace_operation(__IOR__, __OR__, var, right);
			break;
        case TOK_BIT_XOR_ASSIGN:
            perform_inplace_operation(__IXOR__, __XOR__, var, right);
			break;
        case TOK_LEFT_SHIFT_ASSIGN:
            perform_inplace_operation(__ILSHIFT__, __LSHIFT__, var, right);
			break;
        case TOK_RIGHT_SHIFT_ASSIGN:
            perform_inplace_operation(__IRSHIFT__, __ILSHIFT__, var, right);
			break;
        default:
            interpretor_throw_error("Invalid assignment operator %s\n", get_token_type(op));
    }
}


static ClassInstance* evaluate_function_expression(Expr* expr){
    FuncExpr* fexpr = (FuncExpr*)expr;
    FuncArgs args = {0};
    for(int i = 0; i < array_list_size(fexpr->args); i++){
        Expr* param = array_list_get(fexpr->args, Expr*, i);
        ClassInstance* evaluatedParam = evaluate_expression(param);
        func_args_add(&args, evaluatedParam);
    }
    return interpretor_call_function(fexpr->name, args);
}

ClassInstance* evaluate_expression(Expr* expression){
    if(expression == NULL) goto INVALID; 
    switch (expression->type) {
        case EXPR_BINARY:
            return evaluate_binary_expression(expression);
        case EXPR_LITERAL:{
            LiteralExpr* lexpr = (LiteralExpr*)expression;
            //if it is a variable, we get a copy of its value 
            if(lexpr->litType == LIT_IDENTIFIER){
                return interpretor_get_var(lexpr->identifier); 
            }
            return Literal_to_class(lexpr); 
        }
        case EXPR_UNARY:
            return evaluate_unary_expression(expression);
        case EXPR_GROUPING:
            return evaluate_expression(expression->expr);
        case EXPR_FUNC: 
            return evaluate_function_expression(expression);
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
            long start = interpretor_save_expression();
            evaluate_expression(estmt->expr);
            interpretor_restore_expression(start);
            break;
        }
        case STMT_ASSIGN: {
            AssignStmt* astmt = (AssignStmt*)(statement);
            //AS OF RIGHT NOW VARIABLES ARE NOT LAZLILY ASSIGNED
            long start = interpretor_save_expression();
            ClassInstance* value = evaluate_expression(astmt->value);
            interpretor_assign_var(astmt->identifier, value);
            interpretor_restore_expression(start);
            break;
        }
        case STMT_ASSIGN_OP: {
            AssignOpStmt* opstmt = (AssignOpStmt*)(statement);
            long start = interpretor_save_expression();
            evaluate_assignop_stmt(opstmt->identifier, opstmt->op, opstmt->value);
            interpretor_restore_expression(start);
            break;
        }
        case STMT_WHILE:{
            WhileStmt* wstmt = (WhileStmt*)(statement);
            long start = interpretor_save_expression();

            while(fast_as_bool(evaluate_expression(wstmt->condition))){
                interpretor_restore_expression(start);
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
            interpretor_restore_expression(start);
            break;
        }
        case STMT_IF: {
            IfStmt* ifstmt = (IfStmt*)statement;
            long if_start = interpretor_save_expression();
            if(fast_as_bool(evaluate_expression(ifstmt->condition))){
                interpretor_restore_expression(if_start);
                evaluate_statement(ifstmt->then);
            } else {
                //loop through all the elif statements if there are any and evaluate them 
                bool active_elif = false;
                ElifStmt* curr_elif = ifstmt->elif;
                while(curr_elif != NULL){
                    long elif_start = interpretor_save_expression();
                    if(fast_as_bool(evaluate_expression(curr_elif->condition))){
                        interpretor_restore_expression(elif_start);
                        evaluate_statement(curr_elif->then);
                        active_elif = true;
                        break;
                    }
                    interpretor_restore_expression(elif_start);
                    curr_elif = (ElifStmt*)curr_elif->next;
                }
                if(!active_elif && ifstmt->elseBranch != NULL){
                    evaluate_statement(ifstmt->elseBranch);
                }             
                interpretor_restore_expression(if_start);
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
            long assert_start = interpretor_save_expression();
            ClassInstance* temp = evaluate_expression(astmt->condition);
            if(fast_as_bool(temp) == false){
                //don't have exceptions implemented yet, so we will just throw an error
                interpretor_throw_error("AssertionError: %s", (astmt->msg == NULL) ? "" : get_str(astmt->msg));
            }
            interpretor_restore_expression(assert_start);
            break;
        }
        case STMT_FUNC:{
            FunctionStmt* fstmt = (FunctionStmt*)(statement);
            interpretor_create_function(fstmt);
            break;
        }
        case STMT_RETURN: {
            ReturnStmt* rstmt = (ReturnStmt*)(statement);
            ClassInstance* res = evaluate_expression(rstmt->value);
            interpretor_return(res);
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




