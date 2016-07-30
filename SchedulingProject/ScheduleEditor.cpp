#include "stdafx.h"
#include "ScheduleEditor.h"

#include "imgui.h"
#include <algorithm>

#define BACKGROUND_COLOR 0xffffffff
#define LINE_COLOR 0xff333333
#define IDLE_COLOR 0x00cccccc

static float UIScale = 1.0f;
static float jobWidth = 20.0f * UIScale;
static float borderPadding = 40.0f * UIScale;
static float jobSpacing = 5.0f * UIScale;
static float syncLineThickness = 2.0f * UIScale;
static float syncLineRunoff = 15.0f * UIScale;

ScheduleEditor::ScheduleEditor()
{
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


	selectedServer = 0;
	selectedJob = 0;
	selectedJobPtr = NULL;

	selectedSyncPoint = -1;

	snapshot = NULL;
}


ScheduleEditor::~ScheduleEditor()
{
}

void ScheduleEditor::OnGUI(JobData & jd)
{
	if (snapshot)
	{
		DrawJobRun(*snapshot);
	}

	jr = JobRun(jd);
	jr.Simulate();

	ImGuiIO io = ImGui::GetIO();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	ImVec2 reg = ImGui::GetContentRegionAvail();
	ImVec2 tl = ImGui::GetCursorScreenPos();
	ImVec2 br(tl.x + reg.x, tl.y + reg.x);
	ImVec2 tlCorner(tl);


	float timeScale = reg.x * UIScale / 5.0f / VAL_DEF / 10.0f;

	dl->AddRectFilled(tl, ImVec2(tl.x + reg.x, tl.y + (2.0f * borderPadding) + (jobSpacing * (jd.jobs.size() - 1)) + (jobWidth * jd.jobs.size())), BACKGROUND_COLOR);

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
						//saves.push(jd);
						selectedSyncPoint = i;
					}
				}
			}
			else if (selectedSyncPoint == i)
			{
				syncPoint += io.MouseDelta.x / timeScale;

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

	//ImGui::ColorEdit3("Job color", colors[selectedJob]);

	if (selectedJobPtr)
	{
		float jobTime = floorf((float)*selectedJobPtr) / VAL_DEF;
		ImGui::InputFloat("Job time", &jobTime, 0.05f, 0.25f);
		*selectedJobPtr = jobTime * VAL_DEF;
	}

	ImGuiStyle style = ImGui::GetStyle();

	float itemWidth = (reg.x - (8.0f * style.ItemSpacing.x)) / 8.0f;

	if (ImGui::Button("Capture", ImVec2(itemWidth, 0)))
	{
		if (snapshot)
		{
			delete snapshot;
			snapshot = NULL;
		}
		snapshot = new JobRun(jd);
	}
	ImGui::SameLine();
	if (ImGui::Button("Hide", ImVec2(itemWidth, 0)))
	{
		if (snapshot)
		{
			delete snapshot;
			snapshot = NULL;
		}
	}
}


void ScheduleEditor::DrawJobRun(JobRun & jobRun)
{
	ImVec2 reg = ImGui::GetContentRegionAvail();
	ImDrawList* dl = ImGui::GetWindowDrawList();

	float timeScale = reg.x * UIScale / 5.0f / VAL_DEF / 10.0f;

	ImVec2 tl = ImGui::GetCursorScreenPos();
	ImVec2 tlCorner(tl);


	dl->AddRectFilled(tl, ImVec2(tl.x + reg.x, tl.y + (2.0f * borderPadding) + (jobSpacing * (jobRun.data.jobs.size() - 1)) + (jobWidth * jobRun.data.jobs.size())), BACKGROUND_COLOR);

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