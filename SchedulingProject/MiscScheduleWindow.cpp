#include "stdafx.h"
#include "MiscScheduleWindow.h"

#include "imgui.h"


MiscScheduleWindow::MiscScheduleWindow() : listener(this)
{
}


MiscScheduleWindow::~MiscScheduleWindow()
{
}

void MiscScheduleWindow::Push(JobData & jd)
{
	jd.isDirty = true;
	undoes.push(jd);

	if (undoes.size() > 200)
	{
		stack<JobData> temp;

		while (undoes.size() > 50)
		{
			temp.push(undoes.top());
			undoes.pop();
		}
		while (undoes.size() > 0)
		{
			undoes.pop();
		}
		while (temp.size())
		{
			undoes.push(temp.top());
			temp.pop();
		}
	}

	while (redoes.size())
	{
		redoes.pop();
	}
}

ScheduleChangeListener* MiscScheduleWindow::GetListener()
{
	return &listener;
}

void MiscScheduleWindow::OnGUI(JobData & jd)
{
	bool undoVisible = false;
	if (undoes.size())
	{
		undoVisible = true;
		if (ImGui::Button("Undo"))
		{
			jd.isDirty = true;
			redoes.push(jd);
			jd = undoes.top();
			undoes.pop();
		}
	}
	if (redoes.size())
	{
		if (undoVisible)
		{
			ImGui::SameLine();
		}
		if (ImGui::Button("Redo"))
		{
			jd.isDirty = true;
			undoes.push(jd);
			jd = redoes.top();
			redoes.pop();
		}
	}
}