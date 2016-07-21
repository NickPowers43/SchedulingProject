
#include "stdafx.h"
// ImGui - standalone example application for Glfw + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "JobData.h"
#include "JobRun.h"
#include "ScheduleViewer.h"
#include <Windows.h>

using namespace std;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

double currentFrame;
double lastFrame;

int main(int, char**)
{

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Schedule Simulator", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Setup ImGui binding
    ImGui_ImplGlfw_Init(window, true);

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

	ScheduleViewer sv;
	ScheduleViewer sv2;

	lastFrame = glfwGetTime();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();

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

		sv.OnGUI();
		//sv2.OnGUI();

		ImGui::End();

        // Rendering
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);

		currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		double sleepTime = (1.0 / 30.0) - deltaTime;
		if (sleepTime > 0.0)
		{
			unsigned long ms = sleepTime * 1000;
			if (ms > 0)
			{
				Sleep(ms);
			}
		}

    }

    // Cleanup
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();

    return 0;
}
