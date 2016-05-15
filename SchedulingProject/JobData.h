#pragma once

#include <vector>

using namespace std;

class JobData
{
public:
	//JobData();
	JobData(vector<vector<float>> jobs);
	~JobData();

	vector<vector<float>> jobs;
};

