#pragma once


#include <vector>

using namespace std;

class JobData
{
public:
	JobData();
	JobData(const JobData& jd);
	JobData(vector<vector<ValType>> jobs, vector<ValType> syncPoints);
	~JobData();

	void SaveToFile(string filePath);
	void MinimizeIdletime();
	static JobData LoadFromFile(string filePath);

	vector<vector<ValType>> jobs;
	vector<ValType> syncPoints;
};

