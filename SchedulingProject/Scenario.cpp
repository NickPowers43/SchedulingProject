#include "stdafx.h"
#include "Scenario.h"

#include <Windows.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

#include "Min.h"

using namespace std;

Scenario::Scenario()
{
	isDirty = true;
}
Scenario::Scenario(const Scenario & scenario) : jobs(jd.jobs), syncPoints(jd.syncPoints)
{
	isDirty = true;
}
Scenario::Scenario(Jobs jobs, vector<ValType> syncPoints) : jobs(jobs), syncPoints(syncPoints)
{
}

void Scenario::SaveToFile(string filePath)
{
	ofstream ofs(filePath.c_str(), ifstream::out);

	ofs << "v2" << endl;
	ofs << jobs.serverCount() << endl;

	for (size_t i = 0; i < jobs.serverCount(); i++)
	{
		ofs << jobs.jobCount(i) << endl;
		for (size_t j = 0; j < jobs[0].size(); j++)
		{
			ofs << jobs.getJob(i, j) << endl;
		}
	}

	ofs << syncPoints.size() << endl;
	for (size_t i = 0; i < syncPoints.size(); i++)
	{
		ofs << syncPoints[i] << endl;
	}
}


void MinIdletimeAux(vector<bool> crossing, vector<vector<ValType>> jobs, vector<ValType> & syncPoints)
{
	for (size_t i = 0; i < jobs.size(); i++)
	{

	}
}
void MinIdletimeSection(vector<vector<ValType>> jobs, vector<ValType> & syncPoints)
{
	Min<ValType, vector<ValType>> minConfig(VAL_ZERO, syncPoints);

	for (size_t sectionI = 0; sectionI < jobs[0].size(); sectionI++)
	{
		ValType initialIdletime;

	}
}
void MinIdletime(vector<vector<ValType>> jobs, vector<ValType> & syncPoints)
{
}
void Scenario::MinimizeIdletime()
{
	if (syncPoints.size() == jobs[0].size())
	{
		
	}
	else
	{

	}
}
Scenario Scenario::LoadFromFile(string filePath)
{
	ifstream ifs(filePath.c_str(), ifstream::in);

	string line;

	getline(ifs, line);
	
	if (line.compare("v2") == 0)
	{
		getline(ifs, line);
		int serverCount = stoi(line);

		vector<vector<ValType>> jobs;
		for (size_t i = 0; i < serverCount; i++)
		{
			getline(ifs, line);
			int jobCount = stoi(line);

			jobs.push_back(vector<ValType>());
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

		return Scenario(jobs, syncPoints);
	}
	else
	{
		int serverCount = stoi(line);
		getline(ifs, line);
		int jobCount = stoi(line);

		vector<vector<ValType>> jobs;
		for (size_t i = 0; i < serverCount; i++)
		{
			jobs.push_back(vector<ValType>());
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

		return Scenario(jobs, syncPoints);
	}
}

Scenario::~Scenario()
{
}
