#include "stdafx.h"
#include "Jobs.h"



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

		if (jobs[j].size())
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
