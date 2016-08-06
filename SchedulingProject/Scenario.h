#pragma once


#include <vector>

using namespace std;

class Scenario
{
public:
	Scenario();
	Scenario(const Scenario& jd);
	Scenario(vector<vector<ValType>> jobs, vector<ValType> syncPoints);
	~Scenario();

	void SaveToFile(string filePath);
	void MinimizeIdletime();
	static Scenario LoadFromFile(string filePath);

	vector<vector<ValType>> jobs;
	vector<ValType> syncPoints;

	bool isDirty;
};

