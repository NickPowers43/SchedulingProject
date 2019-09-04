#pragma once

#include <vector>

using namespace std;

class Jobs
{
public:
	Jobs();
	Jobs(vector<vector<ValType>> jobs);
	~Jobs();

	int serverCount();
	int jobCount(int server);

	ValType getJob(int server, int index);
	void setJob(int server, int index, ValType value);

	void split(Jobs &first, Jobs &remaining, int amount);
	void add(Jobs &range);
	ValType maxRunTime();

	void addServer();
	void addJob(int server, ValType length);
	void removeServer(int server);
	void removeJob(int server, int index);

	void jobsAfter(ValType syncPoint, Jobs & output, ValType & idleTime, bool useT);
	size_t MaxJobCount();

private:
	vector<vector<ValType>> jobs;
};

