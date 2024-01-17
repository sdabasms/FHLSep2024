//
// pch.h
//

#pragma once

#include "gtest/gtest.h"

// For simplicity, include all SE/QP headers here.
//

#include "storeng/btreesession.h"
#include "storeng/btree.h"

#include "queryprocessing/aggregate.h"
#include "queryprocessing/btreescanner.h"
#include "queryprocessing/constscan.h"
#include "queryprocessing/filter.h"
#include "queryprocessing/join.h"
#include "queryprocessing/project.h"
#include "queryprocessing/sort.h"


// Executes a query and runs the expression for every row returned.
// You need to pass an adequately sized Value* array.
// The expression can be anything, it can print the rows, it can count, it can have other side effects.
//
void ExecuteQuery(Qp::IOperator *root, Value *rgvals, ProjectExpression expression = nullptr);