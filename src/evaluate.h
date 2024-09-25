#pragma once 
#include "object.h"
#include "statement.h"
#include "expression.h"


ClassInstance* evaluate_expression(Expr* expression);

void evaluate_statement(Statement* statement);
