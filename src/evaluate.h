#pragma once 
#include "interpret.h"
#include "statement.h"
#include "expression.h"


LiteralExpr* evaluate_expression(Interpretor *interpret, Expr* expression);

void evaluate_statement(Interpretor *interpret, Statement* statement);
