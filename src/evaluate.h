#pragma once 
#include "interpret.h"
#include "object.h"
#include "statement.h"
#include "expression.h"


ClassInstance* evaluate_expression(Interpretor *interpret, Expr* expression);

void evaluate_statement(Interpretor *interpret, Statement* statement);
