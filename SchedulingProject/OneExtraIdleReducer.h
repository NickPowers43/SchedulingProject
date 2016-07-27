#pragma once

#include "IdleReducer.h"

class OneExtraIdleReducer :
	public IdleReducer
{
public:
	OneExtraIdleReducer();
	~OneExtraIdleReducer();

	ReduceResults Reduce(vector<vector<ValType>> jobs, size_t syncPointCount);
};

