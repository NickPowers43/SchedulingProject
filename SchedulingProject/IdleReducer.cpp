#include "stdafx.h"
#include "IdleReducer.h"


IdleReducer::IdleReducer()
{
}


IdleReducer::~IdleReducer()
{
}

bool IdleReducer::Cancelled()
{
	return cancelled;
}
bool IdleReducer::Running()
{
	return running;
}

float IdleReducer::GetProgress()
{
	return progress;
}
ReduceResults IdleReducer::GetResult()
{
	return result;
}

void IdleReducer::Cancel()
{
	cancelled = true;
}

void IdleReducer::SetProgress(float progress)
{
	IdleReducer::progress = progress;
}

void IdleReducer::Reduce(Jobs jobs, size_t syncPointCount, ValType t)
{
}
void IdleReducer::OnGUI()
{
}