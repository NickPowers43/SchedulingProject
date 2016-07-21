#include "stdafx.h"
#include "ScheduleViewer.h"

#include "imgui.h"
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

#define VAL_ZERO 0
#define VAL_INF INT_MAX

#define FILEPATH_BUF_SIZE 256

#define BACKGROUND_COLOR 0xffffffff
#define LINE_COLOR 0xff333333
#define IDLE_COLOR 0x00cccccc

static float UIScale = 1.0f;
static float jobWidth = 20.0f * UIScale;
static float borderPadding = 40.0f * UIScale;
static float jobSpacing = 5.0f * UIScale;
static float syncLineThickness = 2.0f * UIScale;
static float syncLineRunoff = 15.0f * UIScale;

static ValType CalculateOptimal(vector<vector<ValType>> jobs, vector<ValType>& syncPoints, int remainingSyncPoints)
{
	bool ranOnce = false;
	ValType bestSyncPoint = VAL_INF;
	ValType bestIdleTime = VAL_INF;
	for (size_t i = 0; i < jobs.size(); i++)
	{
		if (jobs[i].size() > 1)
		{
			ranOnce = true;
			if (remainingSyncPoints == 0)
			{
				return VAL_INF;
			}

			ValType idleTime = VAL_ZERO;
			vector<vector<ValType>> jobs2;
			for (size_t j = 0; j < jobs.size(); j++)
			{
				if (jobs[j].size() > 1)
				{
					jobs2.push_back(vector<ValType>());
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

			vector<ValType> temp;
			ValType subIdleTime = CalculateOptimal(jobs2, temp, remainingSyncPoints - 1);
			if (subIdleTime != VAL_INF)
			{
				idleTime += subIdleTime;

				if (idleTime < bestIdleTime)
				{
					bestSyncPoint = jobs[i].front();
					bestIdleTime = idleTime;
					syncPoints = temp;
				}
			}

		}
	}

	if (bestSyncPoint != VAL_INF)
	{
		for (size_t i = 0; i < syncPoints.size(); i++)
		{
			syncPoints[i] += bestSyncPoint;
		}
		syncPoints.push_back(bestSyncPoint);
		return bestIdleTime;
	}
	if (ranOnce)
	{
		return VAL_INF;
	}
	else
	{
		return VAL_ZERO;
	}

}
bool CompareFirstElements(vector<ValType> & a, vector<ValType> & b)
{
	return a.front() < b.front();
}
static void FiniteCasesAux(vector<vector<ValType>> jobs, int remainingSyncPoints, long & count)
{
	sort(jobs.begin(), jobs.end(), CompareFirstElements);

	bool ranOnce = false;
	for (size_t i = 0; i < jobs.size(); i++)
	{
		bool skip = (i > 0) ? jobs[i].front() == jobs[i-1].front() : false;
		if (jobs[i].size() > 1 && !skip)
		{
			ranOnce = true;
			if (remainingSyncPoints == 0)
			{
				return;
			}

			//in this context we have chosen the end of the ith job to place
			//a syncronization point. we now slice the jobs to create a
			//sub-problem to solve.
			vector<vector<ValType>> jobs2;
			for (size_t j = 0; j < jobs.size(); j++)
			{
				if (jobs[j].size() > 1)
				{
					jobs2.push_back(vector<ValType>());
					if (j != i)
					{
						if (jobs[j].front() > jobs[i].front())
						{
							jobs2.back().push_back(jobs[j].front() - jobs[i].front());
						}
					}
					for (size_t k = 1; k < jobs[j].size(); k++)
					{
						jobs2.back().push_back(jobs[j][k]);
					}
				}
			}

			FiniteCasesAux(jobs2, remainingSyncPoints - 1, count);
		}
	}

	if (!ranOnce && (remainingSyncPoints == 0))
	{
		count++;
	}
}
static size_t FiniteCases(vector<vector<ValType>> jobs, int remainingSyncPoints)
{
	long temp = 0;

	FiniteCasesAux(jobs, remainingSyncPoints, temp);

	return temp;
}

ScheduleViewer::ScheduleViewer()
{
	canLoadFile = false;
	filePath = new char[FILEPATH_BUF_SIZE];
	for (size_t i = 0; i < FILEPATH_BUF_SIZE; i++)
	{
		filePath[i] = '\0';
	}

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

	int hues[16] = {
		0,
		164,
		21,
		87,
		212,
		56,
		32,
		130,
		142,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};

	colors = new float*[MAX_JOBS];
	for (size_t i = 0; i < MAX_JOBS; i++)
	{
		ImColor col = ImColor::HSV(255.0f / hues[i], 195.0f / 255.0f, 195.0f / 255.0f);
		ImVec4 comps = col;

		colors[i] = new float[3];
		colors[i][0] = comps.x;
		colors[i][1] = comps.y;
		colors[i][2] = comps.z;
	}

	colorCount = MAX_JOBS;

	snapshot = NULL;

	finiteCases = 0;
}


ScheduleViewer::~ScheduleViewer()
{
}

void ScheduleViewer::OnGUI()
{
	static float syncPointButtonSize = 20.0f;
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

	float timeScale = reg.x * UIScale / 5.0f / 100000.0f;


	stringstream ss;

	dl->AddRectFilled(tl, ImVec2(tl.x + reg.x, tl.y + (2.0f * borderPadding) + (jobSpacing * (serverCount - 1)) + (jobWidth * serverCount)), BACKGROUND_COLOR);

	tlCorner.y += borderPadding;
	float jobChartTop = tlCorner.y;
	int i = 0;
	for_each(jr.data.jobs.begin(), jr.data.jobs.end(), [&](vector<int>& jobs) {

		tlCorner.x = tl.x + borderPadding;
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
				dl->AddRectFilled(ImVec2(preceedingEnd, tlCornerJob.y), ImVec2(tlCornerJob.x, brCorner.y), IDLE_COLOR);
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
		if (i < jr.data.jobs.size() - 1)
		{
			tlCorner.y += jobSpacing;
		}
		i++;
	});
	float jobChartBottom = tlCorner.y;
	tlCorner.y += borderPadding;

	br.y = tlCorner.y;

	float horizontalPos = tl.x + borderPadding;
	dl->AddLine(ImVec2(horizontalPos, jobChartTop - syncLineRunoff), ImVec2(horizontalPos, jobChartBottom + syncLineRunoff), LINE_COLOR, syncLineThickness);
	i = 0;
	for_each(jd.syncPoints.begin(), jd.syncPoints.end(), [&](int& syncPoint) {

		if (io.MouseDown[0])
		{
			if (selectedSyncPoint < 0)
			{
				if ((io.MousePos.y > tl.y && io.MousePos.y < br.y) && (io.MousePos.x > tl.x && io.MousePos.x < br.x))
				{
					float dist = io.MousePos.x - (tl.x + borderPadding + (syncPoint * timeScale));
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

		horizontalPos = tl.x + borderPadding + (syncPoint * timeScale);
		dl->AddLine(ImVec2(horizontalPos, jobChartTop - syncLineRunoff), ImVec2(horizontalPos, jobChartBottom + syncLineRunoff), LINE_COLOR, syncLineThickness);
		i++;
	});

	dl->AddRect(tl, ImVec2(tl.x + reg.x, tlCorner.y), LINE_COLOR);

	ImGui::PushItemWidth(110.0f);
	ImGui::SetCursorScreenPos(tl);
	ImGui::InvisibleButton("Background", ImVec2(reg.x, tlCorner.y - tl.y));

	ss.str(string());
	ss << (jr.idleTime * 0.0001f);
	ImGui::LabelText(ss.str().c_str(), "Idle time");

	ss.str(string());
	ss << finiteCases;
	ImGui::LabelText(ss.str().c_str(), "Finite Cases");

	float jobTime = floorf((float)*selectedJobPtr) / 10000;
	updateJobRun |= ImGui::InputFloat("Job time", &jobTime, 0.05f, 0.25f);
	*selectedJobPtr = jobTime * 10000;

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
	if (ImGui::Button("Calculate finite idletime cases"))
	{
		finiteCases = FiniteCases(jd.jobs, jd.syncPoints.size());
	}
	ImGui::PopStyleColor(3);

	ImGui::PopItemWidth();

	ImGui::Separator();

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

	ImGui::Separator();

	ImGui::PushItemWidth(200.0f);
	bool checkFileExists = ImGui::InputText("Path", filePath, FILEPATH_BUF_SIZE);
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		jd.SaveToFile(filePath);
	}
	if (checkFileExists)
	{
		ifstream f(filePath);
		canLoadFile = f.good();
	}
	if (canLoadFile)
	{
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			saves.push(jd);
			jd = JobData::LoadFromFile(filePath);
			updateJobRun = true;
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
		while (saves.size() > 0)
		{
			saves.pop();
		}
		while (temp.size())
		{
			saves.push(temp.top());
			temp.pop();
		}
	}

	if (updateJobRun)
	{
		//update job run
		jr = JobRun(jd);
		finiteCases = -1;
	}
}

void ScheduleViewer::DrawJobRun(JobRun & jobRun)
{
	ImVec2 reg = ImGui::GetContentRegionAvail();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	float timeScale = reg.x * UIScale / 5.0f / 100000.0f;

	ImVec2 tl = ImGui::GetCursorScreenPos();
	ImVec2 tlCorner(tl);


	dl->AddRectFilled(tl, ImVec2(tl.x + reg.x, tl.y + (2.0f * borderPadding) + (jobSpacing * (serverCount - 1)) + (jobWidth * serverCount)), BACKGROUND_COLOR);

	tlCorner.y += borderPadding;
	float jobChartTop = tlCorner.y;
	int i = 0;
	for_each(jobRun.data.jobs.begin(), jobRun.data.jobs.end(), [&](vector<int>& jobs) {

		tlCorner.x = tl.x + borderPadding;

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
				dl->AddRectFilled(ImVec2(preceedingEnd, tlCornerJob.y), ImVec2(tlCornerJob.x, brCorner.y), IDLE_COLOR);
			}
			else
			{
				col.x *= 0.25f;
				col.y *= 0.25f;
				col.z *= 0.25f;
			}

			dl->AddRectFilled(tlCornerJob, brCorner, ImGui::ColorConvertFloat4ToU32(col));

			preceedingEnd = brCorner.x;
			colI = (colI + 1) % MAX_JOBS;
			j++;
		});
		tlCorner.y += jobWidth;
		if (i < jobRun.data.jobs.size() - 1)
		{
			tlCorner.y += jobSpacing;
		}
		i++;
	});
	float jobChartBottom = tlCorner.y;
	tlCorner.y += borderPadding;

	float jobChartB = tlCorner.y;

	float horizontalPos = tl.x + borderPadding;
	dl->AddLine(ImVec2(horizontalPos, jobChartTop - syncLineRunoff), ImVec2(horizontalPos, jobChartBottom + syncLineRunoff), LINE_COLOR, syncLineThickness);
	for_each(jobRun.data.syncPoints.begin(), jobRun.data.syncPoints.end(), [&](int& syncPoint) {

		horizontalPos = tl.x + borderPadding + (syncPoint * timeScale);
		dl->AddLine(ImVec2(horizontalPos, jobChartTop - syncLineRunoff), ImVec2(horizontalPos, jobChartBottom + syncLineRunoff), LINE_COLOR, syncLineThickness);
	});

	dl->AddRect(tl, ImVec2(tl.x + reg.x, tlCorner.y), LINE_COLOR);
	ImGui::InvisibleButton("Background", ImVec2(reg.x, tlCorner.y - tl.y));
}