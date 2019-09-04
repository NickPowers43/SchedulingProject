#include "stdafx.h"
#include "Jobs.h"
#include <algorithm>



Jobs::Jobs()
{
}
Jobs::Jobs(vector<vector<ValType>> jobs) : jobs(jobs)
{
}


Jobs::~Jobs()
{
}

int Jobs::serverCount()
{
	return jobs.size();
}
int Jobs::jobCount(int server)
{
	return jobs[server].size();
}

ValType Jobs::getJob(int server, int index)
{
	return jobs[server][index];
}
void Jobs::setJob(int server, int index, ValType value)
{
	jobs[server][index] = value;
}

void Jobs::split(Jobs &first, Jobs &remaining, int amount)
{
	vector<vector<ValType>> nJobs;
	vector<vector<ValType>> rJobs;
	for (size_t i = 0; i < jobs.size(); i++)
	{
		nJobs.push_back(vector<ValType>());
		int limit = std::min(amount, jobCount(i));
		for (size_t j = 0; j < jobCount(i); j++)
		{
			if (j < amount) {
				nJobs.back().push_back(getJob(i, j));
			}
			else {
				rJobs.back().push_back(getJob(i, j));
			}
		}
	}
	first = Jobs(nJobs);
	remaining = Jobs(rJobs);
}

void Jobs::add(Jobs & last)
{
	for (size_t i = 0; i < serverCount(); i++)
	{
		for (size_t j = 0; j < last.jobCount(i); j++)
		{
			addJob(i, last.getJob(i, j));
		}
	}
}

ValType Jobs::maxRunTime()
{
	//compute T
	ValType m = VAL_ZERO;
	for (size_t i = 0; i < serverCount(); i++)
	{
		ValType sum = VAL_ZERO;
		for (size_t j = 0; j < jobCount(i); j++)
		{
			sum += getJob(i, j);
		}
		if (sum > m)
			m = sum;
	}
	return m;
}

void Jobs::addServer()
{
	jobs.push_back(vector<ValType>());
}
void Jobs::addJob(int server, ValType length)
{
	jobs[server].push_back(length);
}
void Jobs::removeServer(int server)
{
	jobs.erase(jobs.begin() + server);
}
void Jobs::removeJob(int server, int index)
{
	jobs[server].erase(jobs[server].begin() + index);
}

void Jobs::jobsAfter(ValType syncPoint, Jobs & output, ValType & idleTime, bool useT)
{
	idleTime = VAL_ZERO;
	vector<vector<ValType>> jobs2;
	for (size_t j = 0; j < jobs.size(); j++)
	{
		jobs2.push_back(vector<ValType>());

		if(jobs[j].size())
		{
			if (jobs[j].size() < 2)
			{
				if (useT)
				{
					idleTime += syncPoint - jobs[j].front();
				}
				else
				{
					//add nothing
				}
			}
			else
			{
				if (jobs[j].front() > syncPoint)
				{
					jobs2.back().push_back(jobs[j].front() - syncPoint);
				}
				else
				{
					idleTime += syncPoint - jobs[j].front();
				}

				for (size_t k = 1; k < jobs[j].size(); k++)
				{
					jobs2.back().push_back(jobs[j][k]);
				}
			}

		}
		else if(useT)
		{
			//no jobs
			idleTime += syncPoint;
		}
	}

	output = Jobs(jobs2);
}
size_t Jobs::MaxJobCount()
{
	size_t output = 0;
	for (size_t j = 0; j < jobs.size(); j++)
	{
		if (jobs[j].size() > output)
		{
			output = jobs[j].size();
		}
	}
	return output;
}
