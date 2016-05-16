#include "stdafx.h"
#include "JobData.h"


JobData::JobData()
{
}
JobData::JobData(const JobData & jd) : jobs(jd.jobs), syncPoints(jd.syncPoints)
{

}
JobData::JobData(vector<vector<int>> jobs, vector<int> syncPoints) : jobs(jobs), syncPoints(syncPoints)
{
}


JobData::~JobData()
{
}
