#pragma once

#include <vector>

using namespace std;

class JobData
{
public:
	JobData();
	JobData(vector<vector<float>> jobs, vector<float> syncPoints);
	~JobData();

	vector<vector<float>> jobs;
	vector<float> syncPoints;
};

