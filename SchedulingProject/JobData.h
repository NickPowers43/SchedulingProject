#pragma once

#include <vector>

using namespace std;

class JobData
{
public:
	JobData();
	JobData(const JobData& jd);
	JobData(vector<vector<int>> jobs, vector<int> syncPoints);
	~JobData();

	void SaveToFile(string filePath);
	static JobData LoadFromFile(string filePath);

	vector<vector<int>> jobs;
	vector<int> syncPoints;
};

