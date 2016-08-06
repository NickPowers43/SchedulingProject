#pragma once


#include "Jobs.h"

using namespace std;

class Scenario
{
public:
	Scenario();
	Scenario(const Scenario& jd);
	Scenario(Jobs jobs, vector<ValType> syncPoints);
	~Scenario();

	void SaveToFile(string filePath);
	void MinimizeIdletime();
	static Scenario LoadFromFile(string filePath);

	Jobs jobs;
	vector<ValType> syncPoints;

	bool isDirty;
};

