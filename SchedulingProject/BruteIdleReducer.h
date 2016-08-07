#pragma once

#include "IdleReducer.h"

class BruteIdleReducer :
	public IdleReducer
{
public:
	BruteIdleReducer();
	~BruteIdleReducer();

	void Reduce(Jobs jobs, size_t syncPointCount, ValType t);
};

