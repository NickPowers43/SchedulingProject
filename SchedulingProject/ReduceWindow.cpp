#include "stdafx.h"
#include "ReduceWindow.h"

#include "imgui.h"
#include <map>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "BruteIdleReducer.h"
#include "OneExtraIdleReducer.h"

using namespace std;

#define REDUCER_BRUTE 0
#define REDUCER_ONE_EXTRA 1

map<int, IdleReducer*> reducers = {
	{ REDUCER_BRUTE, static_cast<IdleReducer*>(new BruteIdleReducer()) },
	{ REDUCER_ONE_EXTRA, static_cast<IdleReducer*>(new OneExtraIdleReducer()) }
};

ReduceWindow::ReduceWindow()
{
	reducerPreference = REDUCER_BRUTE;

	finiteCases = -1;

	idleTime = VAL_ZERO;

	filePath = new char[FILEPATH_BUF_SIZE];
	for (size_t i = 0; i < FILEPATH_BUF_SIZE; i++)
	{
		filePath[i] = '\0';
	}

	strcpy_s(filePath, FILEPATH_BUF_SIZE, "data.csv");
}


ReduceWindow::~ReduceWindow()
{
}

void ExportCSV(char* filePath, vector<ValType> values)
{
	ofstream ofs(filePath, ifstream::out);

	for (size_t i = 0; i < values.size(); i++)
	{
		ofs << values[i] << endl;
	}

}

void ReduceWindow::OnGUI(JobData & jd)
{
	ImGuiStyle style = ImGui::GetStyle();
	ImVec2 reg = ImGui::GetContentRegionAvail();

	ImGui::PushItemWidth(100.0f);

	stringstream ss;

	ss.str(string());
	ss << (idleTime * 0.0001f);
	ImGui::LabelText(ss.str().c_str(), "Idle time: ");

	ss.str(string());
	ss << finiteCases;
	ImGui::LabelText(ss.str().c_str(), "Finite cases: ");

	ImGui::PopItemWidth();

	ImGui::RadioButton("Brute", &reducerPreference, REDUCER_BRUTE);
	ImGui::RadioButton("One Extra", &reducerPreference, REDUCER_ONE_EXTRA);

	if (ImGui::Button("Reduce"))
	{
		//saves.push(jd);
		//updateJobRun = true;

		ReduceResults results = reducers[reducerPreference]->Reduce(jd.jobs, jd.syncPoints.size());
		jd.syncPoints = results.syncPoints;
		idleTime = results.idleTime;
		//syncPointCount = jd.syncPoints.size();
		finiteCaseTimes = results.finiteCaseTimes;
		finiteCases = finiteCaseTimes.size();

		sort(finiteCaseTimes.begin(), finiteCaseTimes.end());
	}

	ImGui::PushItemWidth((reg.x - style.ItemSpacing.x) * 0.5f);

	ImGui::InputText("Path", filePath, FILEPATH_BUF_SIZE);
	ImGui::SameLine();
	if (ImGui::Button("Export CSV"))
	{
		ExportCSV(filePath, finiteCaseTimes);
	}

	ImGui::PopItemWidth();
}
