#pragma once

#include "IdleReducer.h"

class BruteIdleReducer :
	public IdleReducer
{
public:
	BruteIdleReducer();
	~BruteIdleReducer();

	void Reduce(vector<vector<ValType>> jobs, size_t syncPointCount);
};

