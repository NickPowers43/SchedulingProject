#include "stdafx.h"
#include "ScheduleModificationWindow.h"

#include "imgui.h"

#include <random>

ScheduleModificationWindow::ScheduleModificationWindow()
{
	uniformMin = 1.0f;
	uniformMax = 2.0f;
}


ScheduleModificationWindow::~ScheduleModificationWindow()
{
}

void ScheduleModificationWindow::RandomizePoisson(JobData & jd)
{

}

void ScheduleModificationWindow::RandomizeUniform(JobData & jd)
{
	ImGuiStyle style = ImGui::GetStyle();
	float itemWidth = ((ImGui::GetContentRegionAvailWidth() * 0.5f) - (2.0f * style.ItemSpacing.x)) / 3.0f;

	ImGui::PushItemWidth(itemWidth);

	ImGui::InputFloat("##Min", &uniformMin);
	ImGui::SameLine();
	ImGui::InputFloat("##Max", &uniformMax);
	ImGui::SameLine();

	if (ImGui::Button("Uniform"))
	{
		default_random_engine generator(rand());
		uniform_real_distribution<float> dist(uniformMin, uniformMax);

		for (size_t i = 0; i < jd.jobs.size(); i++)
		{
			for (size_t j = 0; j < jd.jobs[i].size(); j++)
			{
				jd.jobs[i][j] = dist(generator) * VAL_DEF;
			}
		}
	}

	ImGui::PopItemWidth();
}

void ScheduleModificationWindow::OnGUI(JobData & jd)
{
	bool modified = false;
	JobData originalJD;

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

		if (jd.jobs[0].size() != jobCount)
		{
			originalJD = jd;
			modified = true;
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

	if (jd.jobs.size() != serverCount)
	{
		originalJD = jd;
		modified = true;
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

	if (jd.syncPoints.size() != syncPointCount)
	{
		originalJD = jd;
		modified = true;
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

	if (modified)
	{
		//modifyCallback(originalJD);
		jd.isDirty = true;
	}

	RandomizeUniform(jd);
	RandomizePoisson(jd);
}
