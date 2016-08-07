#pragma once

#include "Jobs.h"
#include <vector>

using namespace std;

struct ReduceResults
{
	ValType idleTime;
	vector<ValType> syncPoints;
	size_t casesExplored;
	vector<ValType> finiteCaseTimes;
};

class IdleReducer
{
public:
	IdleReducer();
	virtual ~IdleReducer();

	bool Cancelled();
	bool Running();
	float GetProgress();
	ReduceResults GetResult();
	void Cancel();
	void SetProgress(float progress);
	virtual void Reduce(Jobs jobs, size_t syncPointCount, ValType t);
	virtual void OnGUI();

protected:

	ReduceResults result;
	bool cancelled;
	bool running;
	float progress;
};

