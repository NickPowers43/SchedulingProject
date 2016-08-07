#include "stdafx.h"
#include "ReduceWindow.h"

#include "imgui.h"
#include <map>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <thread>

#include "BruteIdleReducer.h"
#include "OneExtraIdleReducer.h"

using namespace std;

#define REDUCER_BRUTE 0
#define REDUCER_ONE_EXTRA 1

map<int, IdleReducer*> reducers = {
	{ REDUCER_BRUTE, static_cast<IdleReducer*>(new BruteIdleReducer()) },
	{ REDUCER_ONE_EXTRA, static_cast<IdleReducer*>(new OneExtraIdleReducer()) }
};

ReduceWindow::ReduceWindow(ScheduleChangeListener* changeListener) : changeListener(changeListener)
{
	reducerPreference = REDUCER_BRUTE;

	finiteCases = -1;
	totalCases = -1;

	idleTime = VAL_ZERO;

	filePath = new char[FILEPATH_BUF_SIZE];
	for (size_t i = 0; i < FILEPATH_BUF_SIZE; i++)
	{
		filePath[i] = '\0';
	}

	strcpy_s(filePath, FILEPATH_BUF_SIZE, "data.csv");

	activeReducer = NULL;
}


ReduceWindow::~ReduceWindow()
{
}

void ExportCSV(char* filePath, vector<ValType> values)
{
	ofstream ofs(filePath, ifstream::out);

	for (size_t i = 0; i < values.size(); i++)
	{
		ofs << (values[i]  / (float)VAL_DEF) << endl;
	}

}

void ReduceWindow::OnGUI(Scenario & scenario)
{
	ImGuiStyle style = ImGui::GetStyle();
	ImVec2 reg = ImGui::GetContentRegionAvail();

	float windowWidth = (ImGui::GetWindowContentRegionWidth() * 0.5f) - style.FramePadding.x;

	//
	ImGui::BeginChild("Statistics", ImVec2(windowWidth, reg.y), true);
	{
		ImGui::PushItemWidth(100.0f);

		stringstream ss;

		ss.str(string());
		ss << (idleTime * 0.0001f);
		ImGui::LabelText(ss.str().c_str(), "Idle time: ");

		ss.str(string());
		ss << finiteCases;
		ImGui::LabelText(ss.str().c_str(), "Finite cases: ");

		ss.str(string());
		ss << totalCases;
		ImGui::LabelText(ss.str().c_str(), "Total cases: ");

		ImGui::PopItemWidth();
	}
	ImGui::EndChild();
	//

	ImGui::SameLine();

	//
	ImGui::BeginChild("Modification Window", ImVec2(windowWidth, reg.y), true);
	{
		ImGui::RadioButton("Brute", &reducerPreference, REDUCER_BRUTE);
		ImGui::RadioButton("One Extra", &reducerPreference, REDUCER_ONE_EXTRA);

		if (activeReducer)
		{
			if (activeReducer->Running())
			{
				if (ImGui::Button("Cancel"))
				{
					activeReducer->Cancel();
				}

				//draw progress bar
				ImVec2 size(ImGui::GetContentRegionAvailWidth(), 25.0f);
				ImVec2 pos = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + (size.x * 0.5f), pos.y + size.y), 0xffffffff); // fill
				ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), 0xffffffff); // border
				ImGui::Dummy(size);
			}
			else
			{
				////get result
				//ReduceResults results = activeReducer->GetResult();
				//jd.syncPoints = results.syncPoints;
				//idleTime = results.idleTime;
				////syncPointCount = jd.syncPoints.size();
				//finiteCaseTimes = results.finiteCaseTimes;
				//finiteCases = finiteCaseTimes.size();
				//totalCases = results.casesExplored;
				//jd.isDirty = true;


				activeReducer = NULL;
				reducerThread.join();
			}
		}
		else
		{
			if (ImGui::Button("Reduce"))
			{
				changeListener->Push(scenario);

				/*activeReducer = reducers[reducerPreference];

				reducerThread = thread([&]() {
					activeReducer->Reduce(jd.jobs, jd.syncPoints.size());
				});*/
			}
		}


		//ImGui::PushItemWidth((windowWidth - style.ItemSpacing.x) * 0.5f);

		ImGui::InputText("##Path", filePath, FILEPATH_BUF_SIZE);
		ImGui::SameLine();
		if (ImGui::Button("Export CSV"))
		{
			ExportCSV(filePath, finiteCaseTimes);
		}

		//ImGui::PopItemWidth();
	}

	ImGui::EndChild();
	//

}
