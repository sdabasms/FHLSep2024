#pragma once

#include "common/value.h"

#include <functional>

// A project expression takes a row and modifies any values.
//
typedef std::function<void(Value*)> ProjectExpression;

// A boolean expression evaluates an expression on a row and returns
// whether the expression is true or false for that row.
//
typedef std::function<bool(Value *)> BooleanExpression;

// A join expression takes two rows (left and right) as input
// and returns whether they are a match.
//
typedef std::function<bool(Value *, Value *)> JoinExpression;

// An aggregate expression takes an input row and and an output row and
// aggregates the input row into the output row.
// A bool param indicates whether the aggregation is (re)starting,
// in which case the aggregate expression needs to reset its values to start again at 0.
//
typedef std::function<void(Value *, Value *, bool)> AggregateExpression;

// A compare expression compares two rows and returns whether the first sorts lower than the second.
//
typedef std::function<bool(Value *, Value *)> CompareExpression;
