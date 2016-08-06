#pragma once

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
	virtual void Reduce(vector<vector<ValType>> jobs, size_t syncPointCount);

protected:

	ReduceResults result;
	bool cancelled;
	bool running;
	float progress;
};

