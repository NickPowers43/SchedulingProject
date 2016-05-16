#include "stdafx.h"
#include "ScheduleViewer.h"

#include "imgui.h"
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;

static float CalculateOptimal(vector<vector<int>> jobs, vector<int>& syncPoints, int remainingSyncPoints)
{
	syncPoints = vector<int>();

	bool ranOnce = false;
	int bestSyncPoint = INT_MAX;
	int best = INT_MAX;
	vector<int> bestPoints;
	for (size_t i = 0; i < jobs.size(); i++)
	{
		if (jobs[i].size() > 1)
		{
			ranOnce = true;
			if (!remainingSyncPoints)
			{
				return INT_MAX;
			}

			float idleTime = 0.0f;
			vector<vector<int>> jobs2;
			for (size_t j = 0; j < jobs.size(); j++)
			{
				if (jobs[j].size() > 1)
				{
					jobs2.push_back(vector<int>());
					if (j != i)
					{
						if (jobs[j].front() > jobs[i].front())
						{
							jobs2.back().push_back(jobs[j].front() - jobs[i].front());
						}
						else
						{
							idleTime += jobs[i].front() - jobs[j].front();
						}
					}
					for (size_t k = 1; k < jobs[j].size(); k++)
					{
						jobs2.back().push_back(jobs[j][k]);
					}
				}
			}

			vector<int> temp;
			idleTime += CalculateOptimal(jobs2, temp, remainingSyncPoints - 1);

			if (idleTime < best)
			{
				bestSyncPoint = jobs[i][0];
				best = idleTime;
				bestPoints = temp;
			}
		}
	}

	if (bestSyncPoint != INT_MAX)
	{
		for (size_t i = 0; i < bestPoints.size(); i++)
		{
			bestPoints[i] += bestSyncPoint;
		}
		bestPoints.push_back(bestSyncPoint);
		syncPoints = bestPoints;
		return best;
	}
	if (ranOnce)
	{
		return INT_MAX;
	}
	else
	{
		return 0.0f;
	}

}

ScheduleViewer::ScheduleViewer()
{
	vector<int> syncPoints;
	syncPoints.push_back(20000);
	syncPoints.push_back(40000);
	syncPoints.push_back(60000);
	syncPoints.push_back(80000);

	vector<vector<int>> exJobs;
	exJobs.push_back(vector<int>());
	exJobs.back().push_back(35500);
	exJobs.back().push_back(10000);
	exJobs.back().push_back(10000);
	exJobs.push_back(vector<int>());
	exJobs.back().push_back(16000);
	exJobs.back().push_back(16000);
	exJobs.back().push_back(34700);
	exJobs.push_back(vector<int>());
	exJobs.back().push_back(10000);
	exJobs.back().push_back(38500);
	exJobs.back().push_back(10000);

	jd = JobData(exJobs, syncPoints);

	jr = JobRun(jd);

	selectedServer = 0;
	selectedJob = 0;
	selectedJobPtr = &jd.jobs[0][0];

	selectedSyncPoint = -1;

	serverCount = jd.jobs.size();
	jobCount = jd.jobs[0].size();
	syncPointCount = syncPoints.size();

	colors = new float*[MAX_JOBS];
	for (size_t i = 0; i < MAX_JOBS; i++)
	{
		ImColor col = ImColor::HSV(i / (float)MAX_JOBS, 0.7f, 0.7f);
		colors[i] = new float[3];
		ImVec4 comps = col;
		colors[i][0] = comps.x;
		colors[i][1] = comps.y;
		colors[i][2] = comps.z;
	}

	colorCount = MAX_JOBS;

	snapshot = NULL;
}


ScheduleViewer::~ScheduleViewer()
{
}

void ScheduleViewer::OnGUI()
{
	static float jobWidth = 20.0f;
	static float syncPointButtonSize = 20.0f;
	static float vPadding = 5.0f;
	static int colorJobHover = 0xffffffff;

	ImGuiIO io = ImGui::GetIO();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	bool updateJobRun = false;
	serverCount = jd.jobs.size();
	if (updateJobRun |= ImGui::SliderInt("Servers", &serverCount, 1, 16))
		saves.push(jd);
	jobCount = jd.jobs[0].size();
	if (updateJobRun |= ImGui::SliderInt("Jobs", &jobCount, 1, MAX_JOBS))
		saves.push(jd);


	while (jd.jobs.size() > serverCount)
	{
		jd.jobs.pop_back();
	}
	while (jd.jobs.size() < serverCount)
	{
		jd.jobs.push_back(vector<int>());
		for (size_t i = 0; i < jobCount; i++)
		{
			jd.jobs.back().push_back(10000);
		}
	}

	for_each(jd.jobs.begin(), jd.jobs.end(), [&](vector<int>& jobs) {
		while (jobs.size() > jobCount)
		{
			jobs.pop_back();
		}
		while (jobs.size() < jobCount)
		{
			jobs.push_back(10000);
		}
	});

	if (snapshot)
	{
		DrawJobRun(*snapshot);
	}

	//Draw working schedule

	//get the boundaries of this item
	ImVec2 reg = ImGui::GetContentRegionAvail();
	ImVec2 tl = ImGui::GetCursorScreenPos();
	ImVec2 br(tl.x + reg.x, tl.y + reg.x);
	ImVec2 tlCorner(tl);

	float timeScale = reg.x / 5.0f / 100000.0f;

	tlCorner.y += vPadding;

	stringstream ss;

	int i = 0;
	for_each(jr.data.jobs.begin(), jr.data.jobs.end(), [&](vector<int>& jobs) {

		tlCorner.x = tl.x;
		/*ss.str(string());
		ss << i;
		dl->AddText(tlCorner, 0xffffffff, ss.str().c_str());
		tlCorner.x += jobWidth;*/
		int j = 0;
		int colI = 0;
		float preceedingEnd = tlCorner.x;
		for_each(jobs.begin(), jobs.end(), [&](int& job) {

			float jobLength = job * timeScale;

			ImVec4 col;
			col.x = colors[colI][0];
			col.y = colors[colI][1];
			col.z = colors[colI][2];
			col.w = 1.0f;

			ImVec2 tlCornerJob(tlCorner.x + (jr.jobStarts[i][j] * timeScale), tlCorner.y);
			ImVec2 brCorner(tlCornerJob.x + jobLength, tlCornerJob.y + jobWidth);


			if (j < jr.lastJob[i])
			{
				//draw idle time before this job
				dl->AddRectFilled(ImVec2(preceedingEnd, tlCornerJob.y), ImVec2(tlCornerJob.x, brCorner.y), 0xffcccccc);
			}
			else
			{
				col.x *= 0.25f;
				col.y *= 0.25f;
				col.z *= 0.25f;
			}


			if ((io.MousePos.x > tlCornerJob.x && io.MousePos.x < brCorner.x) && (io.MousePos.y > tlCornerJob.y && io.MousePos.y < brCorner.y))
			{
				col.x *= 2.0f;
				col.y *= 2.0f;
				col.z *= 2.0f;
				if (io.MouseClicked[0])
				{
					selectedServer = i;
					selectedJob = j;
					selectedJobPtr = &jd.jobs[i][j];
				}
			}

			dl->AddRectFilled(tlCornerJob, brCorner, ImGui::ColorConvertFloat4ToU32(col));

			if (i == selectedServer && j == selectedJob)
			{
				//draw white border
				dl->AddRect(tlCornerJob, brCorner, 0xffffffff);
			}

			preceedingEnd = brCorner.x;
			colI = (colI + 1) % MAX_JOBS;
			j++;
		});
		tlCorner.y += jobWidth;
		tlCorner.y += vPadding;
		i++;
	});

	br.y = tlCorner.y;

	i = 0;
	for_each(jd.syncPoints.begin(), jd.syncPoints.end(), [&](int& syncPoint) {

		if (io.MouseDown[0])
		{
			if (selectedSyncPoint < 0)
			{
				if ((io.MousePos.y > tl.y && io.MousePos.y < br.y) && (io.MousePos.x > tl.x && io.MousePos.x < br.x))
				{
					float dist = io.MousePos.x - (tl.x + (syncPoint * timeScale));
					if (dist < 0.0f)
						dist = -dist;
					if (dist <= 4.0f)
					{
						saves.push(jd);
						selectedSyncPoint = i;
					}
				}
			}
			else if (selectedSyncPoint == i)
			{
				syncPoint += io.MouseDelta.x / timeScale;
				updateJobRun = true;

				for (size_t j = 0; j < jd.syncPoints.size(); j++)
				{
					if (i != j)
					{
						if (jd.syncPoints[j] > jd.syncPoints[i])
						{
							float temp2 = jd.syncPoints[i];
							jd.syncPoints[i] = jd.syncPoints[j];
							jd.syncPoints[i] = temp2;
						}
					}
				}
			}
		}
		else
		{
			if (selectedSyncPoint == i)
			{
				updateJobRun = true;
				selectedSyncPoint = -1;
			}
		}

		float horizontalPos = tl.x + (syncPoint * timeScale);
		dl->AddLine(ImVec2(horizontalPos, tl.y), ImVec2(horizontalPos, br.y), 0xffffffff);
		i++;
	});

	dl->AddRect(tl, ImVec2(tl.x + reg.x, tlCorner.y), 0xffffffff);

	ImGui::PushItemWidth(110.0f);
	ImGui::SetCursorScreenPos(tl);
	ImGui::InvisibleButton("Background", ImVec2(reg.x, tlCorner.y - tl.y));

	ss.str(string());
	ss << jr.idleTime;
	ImGui::LabelText(ss.str().c_str(), "Idle time");

	updateJobRun |= ImGui::InputInt("Job time", selectedJobPtr, 500, 1000);
	ImGui::SameLine();
	if (ImGui::InputInt("Sync Points", &syncPointCount))
	{
		updateJobRun |= true;
		if (syncPointCount < 1)
		{
			syncPointCount = 1;
		}
		while (jd.syncPoints.size() > syncPointCount)
		{
			jd.syncPoints.pop_back();
		}
		while (jd.syncPoints.size() < syncPointCount)
		{
			if (jd.syncPoints.size())
			{
				jd.syncPoints.push_back(jd.syncPoints.back() + 20000);
			}
			else
			{
				jd.syncPoints.push_back(1.0f);
			}
		}
	}
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(220.0f);

	ImGui::ColorEdit3("Job color", colors[selectedJob]);

	ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Minimize idle time at all costs"))
	{
		saves.push(jd);
		vector<int> temp;
		CalculateOptimal(jd.jobs, temp, INT_MAX);
		jd.syncPoints = temp;
		syncPointCount = jd.syncPoints.size();
		updateJobRun = true;
	}
	ImGui::SameLine();
	ImGui::PopStyleColor(3);

	ImGui::PopItemWidth();
	ImGui::PushItemWidth(220.0f);

	ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Minimize idle time"))
	{
		saves.push(jd);
		vector<int> temp;
		CalculateOptimal(jd.jobs, temp, syncPointCount);
		jd.syncPoints = temp;
		syncPointCount = jd.syncPoints.size();
		updateJobRun = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::PopItemWidth();

	if (updateJobRun)
	{
		//update job run
		jr = JobRun(jd);
	}

	ImGuiStyle &style = ImGui::GetStyle();

	tl = ImGui::GetCursorScreenPos();
	dl->AddLine(ImVec2(tl.x, tl.y), ImVec2(tl.x + reg.x, tl.y), 0xffffffff);
	tl.y += style.ItemSpacing.y;
	tl.y += 15.0f;
	ImGui::SetCursorScreenPos(tl);

	if (ImGui::Button("Undo"))
	{
		if (saves.size())
		{
			jd = saves.top();
			saves.pop();
			jr = JobRun(jd);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Take Snapshot"))
	{
		if (snapshot)
		{
			delete snapshot;
			snapshot = NULL;
		}
		snapshot = new JobRun(jd);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Snapshot"))
	{
		if (snapshot)
		{
			delete snapshot;
			snapshot = NULL;
		}
	}

	if (saves.size() > 100)
	{
		stack<JobData> temp;
		while (saves.size() > 20)
		{
			temp.push(saves.top());
			saves.pop();
		}
		while (temp.size())
		{
			saves.push(temp.top());
			temp.pop();
		}
	}
}

void ScheduleViewer::DrawJobRun(JobRun & jobRun)
{
	ImVec2 reg = ImGui::GetContentRegionAvail();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	static float jobWidth = 20.0f;
	static float vPadding = 5.0f;
	float timeScale = reg.x / 5.0f / 100000.0f;

	ImVec2 tl = ImGui::GetCursorScreenPos();
	ImVec2 tlCorner(tl);

	tlCorner.y += vPadding;

	int i = 0;
	for_each(jobRun.data.jobs.begin(), jobRun.data.jobs.end(), [&](vector<int>& jobs) {

		tlCorner.x = tl.x;

		int j = 0;
		int colI = 0;
		float preceedingEnd = tlCorner.x;
		for_each(jobs.begin(), jobs.end(), [&](int& job) {

			float jobLength = job * timeScale;

			ImVec4 col;
			col.x = colors[colI][0];
			col.y = colors[colI][1];
			col.z = colors[colI][2];
			col.w = 1.0f;

			ImVec2 tlCornerJob(tlCorner.x + (jobRun.jobStarts[i][j] * timeScale), tlCorner.y);
			ImVec2 brCorner(tlCornerJob.x + jobLength, tlCornerJob.y + jobWidth);


			if (j < jobRun.lastJob[i])
			{
				//draw idle time before this job
				dl->AddRectFilled(ImVec2(preceedingEnd, tlCornerJob.y), ImVec2(tlCornerJob.x, brCorner.y), 0xffcccccc);
			}
			else
			{
				col.x *= 0.25f;
				col.y *= 0.25f;
				col.z *= 0.25f;
			}

			dl->AddRectFilled(tlCornerJob, brCorner, ImGui::ColorConvertFloat4ToU32(col));

			if (i == selectedServer && j == selectedJob)
			{
				//draw white border
				dl->AddRect(tlCornerJob, brCorner, 0xffffffff);
			}

			preceedingEnd = brCorner.x;
			colI = (colI + 1) % MAX_JOBS;
			j++;
		});
		tlCorner.y += jobWidth;
		tlCorner.y += vPadding;
		i++;
	});

	float jobChartB = tlCorner.y;

	i = 0;
	for_each(jobRun.data.syncPoints.begin(), jobRun.data.syncPoints.end(), [&](int& syncPoint) {

		float horizontalPos = tl.x + (syncPoint * timeScale);
		dl->AddLine(ImVec2(horizontalPos, tl.y), ImVec2(horizontalPos, jobChartB), 0xffffffff);
		i++;
	});

	dl->AddRect(tl, ImVec2(tl.x + reg.x, tlCorner.y), 0xffffffff);
	ImGui::InvisibleButton("Background", ImVec2(reg.x, tlCorner.y - tl.y));
}