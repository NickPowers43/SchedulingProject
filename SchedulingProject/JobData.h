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

	vector<vector<int>> jobs;
	vector<int> syncPoints;
};

