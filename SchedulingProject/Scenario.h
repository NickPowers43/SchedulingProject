#pragma once


#include "Jobs.h"

using namespace std;

class Scenario
{
public:
	Scenario();
	Scenario(const Scenario& scenario);
	Scenario(Jobs jobs, vector<ValType> syncPoints, ValType t);
	~Scenario();

	void SaveToFile(string filePath);
	static Scenario LoadFromFile(string filePath);

	ValType t;
	Jobs jobs;
	vector<ValType> syncPoints;

	bool isDirty;
};

