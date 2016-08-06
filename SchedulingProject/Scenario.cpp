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
Scenario::Scenario(const Scenario & jd) : jobs(jd.jobs), syncPoints(jd.syncPoints)
{
	isDirty = true;
}
Scenario::Scenario(vector<vector<ValType>> jobs, vector<ValType> syncPoints) : jobs(jobs), syncPoints(syncPoints)
{
}

void Scenario::SaveToFile(string filePath)
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

Scenario::~Scenario()
{
}
