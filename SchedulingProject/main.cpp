
#include "stdafx.h"
// ImGui - standalone example application for Glfw + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "JobData.h"
#include "JobRun.h"
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

static float CalculateOptimal(vector<vector<float>> jobs, vector<float>& syncPoints, int remainingSyncPoints)
{
	syncPoints = vector<float>();

	bool ranOnce = false;
	float bestSyncPoint = INFINITY;
	float best = INFINITY;
	vector<float> bestPoints;
	for (size_t i = 0; i < jobs.size(); i++)
	{
		if (jobs[i].size() > 1)
		{
			ranOnce = true;
			if (!remainingSyncPoints)
			{
				return INFINITY;
			}

			float idleTime = 0.0f;
			vector<vector<float>> jobs2;
			for (size_t j = 0; j < jobs.size(); j++)
			{
				if (jobs[j].size() > 1)
				{
					jobs2.push_back(vector<float>());
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

			vector<float> temp;
			idleTime += CalculateOptimal(jobs2, temp, remainingSyncPoints - 1);

			if (idleTime < best)
			{
				bestSyncPoint = jobs[i][0];
				best = idleTime;
				bestPoints = temp;
			}
		}
	}

	if (bestSyncPoint != INFINITY)
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
		return INFINITY;
	}
	else
	{
		return 0.0f;
	}

}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
    glewInit();

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

	vector<float> syncPoints;
	syncPoints.push_back(0.2f);
	syncPoints.push_back(0.4f);
	syncPoints.push_back(0.6f);
	syncPoints.push_back(0.8f);

	vector<vector<float>> exJobs;
	exJobs.push_back(vector<float>());
	exJobs.back().push_back(0.355f);
	exJobs.back().push_back(0.1f);
	exJobs.back().push_back(0.1f);
	exJobs.push_back(vector<float>());
	exJobs.back().push_back(0.16f);
	exJobs.back().push_back(0.16f);
	exJobs.back().push_back(0.347f);
	exJobs.push_back(vector<float>());
	exJobs.back().push_back(0.1f);
	exJobs.back().push_back(0.385f);
	exJobs.back().push_back(0.1f);
	JobData jd(exJobs);

	JobRun jr(jd, syncPoints);


	int selectedServer = 0;
	int selectedJob = 0;
	float* selectedJobPtr = &jd.jobs[0][0];

	int selectedSyncPoint = -1;

	int serverCount = jd.jobs.size();
	int jobCount = jd.jobs[0].size();
	int syncPointCount = syncPoints.size();

	int colors[2]{ 0xff0000ff, 0xffff0000};
	int colorCount = 2;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);


		static bool opened = true;
		ImGui::SetNextWindowSize(ImVec2(display_w, display_h));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("MainWindow", &opened,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_MenuBar);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Alpha = 1.0f;
		//style.ItemSpacing = ImVec2(0.0f, 0.0f);
		//style.ItemInnerSpacing = ImVec2(0.0f, 0.0f);


		//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		bool updateJobRun = false;
		updateJobRun |= ImGui::SliderInt("Servers", &serverCount, 1, 8);
		updateJobRun |= ImGui::SliderInt("Jobs", &jobCount, 1, 8);
		if (ImGui::InputInt("Sync Points", &syncPointCount))
		{
			updateJobRun |= true;
			if (syncPointCount < 1)
			{
				syncPointCount = 1;
			}
			while (syncPoints.size() > syncPointCount)
			{
				syncPoints.pop_back();
			}
			while (syncPoints.size() < syncPointCount)
			{
				if (syncPoints.size())
				{
					syncPoints.push_back(syncPoints.back() + 0.2f);
				}
				else
				{
					syncPoints.push_back(1.0f);
				}
			}
		}

		ImVec2 reg = ImGui::GetContentRegionAvail();
		//reg.x -= style.WindowPadding.x * 1.0f;
		//reg.y -= style.WindowPadding.y;



		while (jd.jobs.size() > serverCount)
		{
			jd.jobs.pop_back();
		}
		while (jd.jobs.size() < serverCount)
		{
			jd.jobs.push_back(vector<float>());
			for (size_t i = 0; i < jobCount; i++)
			{
				jd.jobs.back().push_back(0.1f);
			}
		}
		for_each(jd.jobs.begin(), jd.jobs.end(), [&](vector<float>& jobs) {
			while (jobs.size() > jobCount)
			{
				jobs.pop_back();
			}
			while (jobs.size() < jobCount)
			{
				jobs.push_back(0.1f);
			}
		});

		ImVec2 subReg = ImGui::GetContentRegionAvail();
		ImDrawList* dl = ImGui::GetWindowDrawList();

		static float bufferWidth = subReg.x - (style.WindowPadding.x * 1.0f);
		float timeScale = bufferWidth / 5.0f;

		ImVec2 tl = ImGui::GetCursorScreenPos();
		ImVec2 br(tl.x + bufferWidth, tl.y + bufferWidth);
		ImVec2 tlCorner(tl);


		ImVec2 bufferSize(bufferWidth, 500);

		static float jobWidth = 20.0f;
		static float syncPointButtonSize = 20.0f;
		static float vPadding = 5.0f;

		static int colorJobHover = 0xffffffff;

		float jobChartT = tl.y + 30.0f;
		tlCorner.y += 30.0f;
		dl->AddLine(ImVec2(tlCorner.x, tlCorner.y), ImVec2(tlCorner.x + bufferWidth, tlCorner.y), 0xffffffff);

		ImGuiIO io = ImGui::GetIO();

		tlCorner.y += vPadding;

		stringstream ss;

		int i = 0;
		for_each(jr.data.jobs.begin(), jr.data.jobs.end(), [&](vector<float>& jobs) {

			tlCorner.x = tl.x;
			/*ss.str(string());
			ss << i;
			dl->AddText(tlCorner, 0xffffffff, ss.str().c_str());
			tlCorner.x += jobWidth;*/
			int j = 0;
			int colI = 0;
			float preceedingEnd = tlCorner.x;
			for_each(jobs.begin(), jobs.end(), [&](float& job) {

				float jobLength = job * timeScale;

				int col;
				ImVec2 tlCornerJob(tlCorner.x + (jr.jobStarts[i][j] * timeScale), tlCorner.y);
				ImVec2 brCorner(tlCornerJob.x + jobLength, tlCornerJob.y + jobWidth);

				col = colors[colI];

				if (j < jr.lastJob[i])
				{
					//draw idle time before this job
					dl->AddRectFilled(ImVec2(preceedingEnd, tlCornerJob.y), ImVec2(tlCornerJob.x, brCorner.y), 0xffcccccc);
				}
				else
				{
					ImColor c(col);
					ImVec4 c2((ImVec4)c);
					c2.x /= 2;
					c2.y /= 2;
					c2.z /= 2;
					c = ImColor(c2);
					col = (ImU32)c;
				}


				if ((io.MousePos.x > tlCornerJob.x && io.MousePos.x < brCorner.x) && (io.MousePos.y > tlCornerJob.y && io.MousePos.y < brCorner.y))
				{
					col |= 0xff3f3f3f;
					if (io.MouseClicked[0])
					{
						selectedServer = i;
						selectedJob = j;
						selectedJobPtr = &jd.jobs[i][j];
					}
				}

				dl->AddRectFilled(tlCornerJob, brCorner, col);

				if (i == selectedServer && j == selectedJob)
				{
					//draw white border
					dl->AddRect(tlCornerJob, brCorner, 0xffffffff);
				}

				preceedingEnd = brCorner.x;
				colI = (colI + 1) % colorCount;
				j++;
			});
			tlCorner.y += jobWidth;
			tlCorner.y += vPadding;
			i++;
		});

		float jobChartB = tlCorner.y;

		i = 0;
		for_each(syncPoints.begin(), syncPoints.end(), [&](float& syncPoint) {

			if (io.MouseDown[0])
			{
				if (selectedSyncPoint < 0)
				{
					if ((io.MousePos.y > tl.y && io.MousePos.y < tlCorner.y) && (io.MousePos.x > tl.x && io.MousePos.x < (tl.x + bufferWidth)))
					{
						float dist = io.MousePos.x - (tl.x + (syncPoint * timeScale));
						if (dist < 0.0f)
							dist = -dist;
						if (dist <= 4.0f)
						{
							selectedSyncPoint = i;
						}
					}
				} else if (selectedSyncPoint == i)
				{
					syncPoint += io.MouseDelta.x / timeScale;
					updateJobRun = true;

					for (size_t j = 0; j < syncPoints.size(); j++)
					{
						if (i != j)
						{
							if (syncPoints[j] > syncPoints[i])
							{
								float temp2 = syncPoints[i];
								syncPoints[i] = syncPoints[j];
								syncPoints[i] = temp2;
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
			dl->AddLine(ImVec2(horizontalPos, jobChartT), ImVec2(horizontalPos, jobChartB), 0xffffffff);
			i++;
		});

		dl->AddRect(tl, ImVec2(tl.x + bufferWidth, tlCorner.y), 0xffffffff);

		ImGui::PushItemWidth(110.0f);
		ImGui::SetCursorScreenPos(tl);
		ImGui::InvisibleButton("Background", ImVec2(subReg.x, tlCorner.y - tl.y));
		ss.str(string());
		ss << jr.idleTime;
		ImGui::LabelText(ss.str().c_str(), "Idle time : ");

		updateJobRun |= ImGui::InputFloat("job time", selectedJobPtr, 0.0075f, 0.05f, 3);

		if (ImGui::Button("Minimize idle time at all costs"))
		{
			vector<float> temp;
			CalculateOptimal(jd.jobs, temp, INT_MAX);
			syncPoints = temp;
			syncPointCount = syncPoints.size();
			updateJobRun = true;
		}
		if (ImGui::Button("Optimize sync point usage"))
		{
			vector<float> temp;
			CalculateOptimal(jd.jobs, temp, syncPointCount);
			syncPoints = temp;
			syncPointCount = syncPoints.size();
			updateJobRun = true;
		}
		if(updateJobRun)
		{
			//update job run
			jr = JobRun(jd, syncPoints);
		}

		ImGui::End();

        // Rendering
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}
