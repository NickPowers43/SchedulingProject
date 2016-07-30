#include "stdafx.h"
#include "ScheduleModificationWindow.h"

#include "imgui.h"

ScheduleModificationWindow::ScheduleModificationWindow()
{
}


ScheduleModificationWindow::~ScheduleModificationWindow()
{
}

void ScheduleModificationWindow::OnGUI(JobData & jd)
{
	ImVec2 reg = ImGui::GetContentRegionAvail();
	ImGuiStyle style = ImGui::GetStyle();

	float itemWidth = (reg.x - (3.0f * style.ItemSpacing.x)) / 4.0f;

	ImGui::PushItemWidth(itemWidth);

	if (jd.jobs.size())
	{
		ImGui::Button("Jobs", ImVec2(itemWidth, 0));
		ImGui::SameLine();


		int jobCount = jd.jobs[0].size();
		ImGui::InputInt("##Jobs", &jobCount, 1, 2);

		if (jobCount < 0)
		{
			jobCount = 0;
		}

		while (jd.jobs[0].size() < jobCount)
		{
			for (size_t i = 0; i < jd.jobs.size(); i++)
			{
				jd.jobs[i].push_back(VAL_DEF);
			}
		}
		while (jd.jobs[0].size() > jobCount)
		{
			for (size_t i = 0; i < jd.jobs.size(); i++)
			{
				if (jd.jobs[i].size())
				{
					jd.jobs[i].pop_back();
				}
			}
		}
	}

	ImGui::Button("Servers", ImVec2(itemWidth, 0));
	ImGui::SameLine();

	int serverCount = jd.jobs.size();
	ImGui::InputInt("##Servers", &serverCount, 1, 2);

	if (serverCount < 0)
	{
		serverCount = 0;
	}

	while (jd.jobs.size() < serverCount)
	{
		jd.jobs.push_back(vector<ValType>());

		if (jd.jobs.size() > 1)
		{
			for (size_t i = 0; i < jd.jobs[0].size(); i++)
			{
				jd.jobs.back().push_back(VAL_DEF);
			}
		}
		else
		{
			jd.jobs[0].push_back(VAL_DEF);
		}
	}
	while (jd.jobs.size() > serverCount)
	{
		if (jd.jobs.size())
		{
			jd.jobs.pop_back();
		}
	}

	ImGui::Button("Sync Points", ImVec2(itemWidth, 0));
	ImGui::SameLine();


	int syncPointCount = jd.syncPoints.size();
	ImGui::InputInt("##SyncPoints", &syncPointCount, 1, 2);

	if (syncPointCount < 0)
	{
		syncPointCount = 0;
	}

	while (jd.syncPoints.size() < syncPointCount)
	{
		jd.syncPoints.push_back((jd.syncPoints.size()) ? jd.syncPoints.back() + VAL_DEF : VAL_DEF);
	}
	while (jd.syncPoints.size() > syncPointCount)
	{
		if (jd.syncPoints.size())
		{
			jd.syncPoints.pop_back();
		}
	}

	ImGui::PopItemWidth();
}
