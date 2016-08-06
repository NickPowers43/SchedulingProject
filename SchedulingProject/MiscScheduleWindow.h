#pragma once

#include <stack>
#include "Scenario.h"

#include "ScheduleChangeListener.h"

class MiscScheduleWindow
{
public:
	MiscScheduleWindow();
	~MiscScheduleWindow();

	void Push(Scenario & jd);
	void OnGUI(Scenario & jd);
	ScheduleChangeListener* GetListener();

	ScheduleChangeListener listener;
	stack<Scenario> undoes;
	stack<Scenario> redoes;
};

