#include "stdafx.h"
#include "JobData.h"


JobData::JobData()
{
}
JobData::JobData(vector<vector<float>> jobs, vector<float> syncPoints) : jobs(jobs), syncPoints(syncPoints)
{
}


JobData::~JobData()
{
}
