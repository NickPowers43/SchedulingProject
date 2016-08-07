#pragma once

#include "IdleReducer.h"

class OneExtraIdleReducer :
	public IdleReducer
{
public:
	OneExtraIdleReducer();
	~OneExtraIdleReducer();

	void Reduce(Jobs jobs, size_t syncPointCount, ValType t);
};

