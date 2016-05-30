#include "stdafx.h"
#include "JobData.h"

#include <Windows.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

JobData::JobData()
{
}
JobData::JobData(const JobData & jd) : jobs(jd.jobs), syncPoints(jd.syncPoints)
{

}
JobData::JobData(vector<vector<int>> jobs, vector<int> syncPoints) : jobs(jobs), syncPoints(syncPoints)
{
}

void JobData::SaveToFile(string filePath)
{
	ofstream ofs(filePath.c_str(), ifstream::out);

	ofs << jobs.size() << endl;
	ofs << jobs[0].size() << endl;

	for (size_t i = 0; i < jobs.size(); i++)
	{
		for (size_t j = 0; j < jobs[0].size(); j++)
		{
			ofs << jobs[i][j] << endl;
		}
	}

	ofs << syncPoints.size() << endl;
	for (size_t i = 0; i < syncPoints.size(); i++)
	{
		ofs << syncPoints[i] << endl;
	}
}
JobData JobData::LoadFromFile(string filePath)
{
	ifstream ifs(filePath.c_str(), ifstream::in);

	string line;
	getline(ifs, line);
	int serverCount = stoi(line);
	getline(ifs, line);
	int jobCount = stoi(line);

	vector<vector<int>> jobs;
	for (size_t i = 0; i < serverCount; i++)
	{
		jobs.push_back(vector<int>());
		for (size_t j = 0; j < jobCount; j++)
		{
			getline(ifs, line);
			jobs[i].push_back(stoi(line));
		}
	}

	getline(ifs, line);
	int sPointCount = stoi(line);

	vector<int> syncPoints;
	for (size_t i = 0; i < sPointCount; i++)
	{
		getline(ifs, line);
		syncPoints.push_back(stoi(line));
	}
	
	return JobData(jobs, syncPoints);
}

JobData::~JobData()
{
}
