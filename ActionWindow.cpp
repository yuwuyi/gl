#include <iostream>
#include "GUI.h"
#include "ActionWindow.h"
#include "ScenarioManager.h"
#include "imgui/imgui.h"
#include "MeshLib_Core/Face.h"
#include "growing/growing.h"

#include "Scenario.h"
#include "Action.h"

extern bool mousePressed[2];
extern ProgStatus progStatus;
extern Face *pickingFace;
extern RegionGrowing *regionGrowing;

ActionWindow::ActionWindow(GLFWwindow *window)
: m_parent(window), m_show(false)
{
}

ActionWindow::~ActionWindow() {

}

void ActionWindow::render() {
    const float window_width = ImGui::GetIO().DisplaySize.x;
    const float window_height = ImGui::GetIO().DisplaySize.y;

    static float fill_alpha = 0.65f;
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScrollBarWidth = 25.0f;
    const ImGuiWindowFlags layout_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar  | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize;

    ImGui::Begin("Action Window", &m_show, ImVec2(window_width / 3,200), fill_alpha, layout_flags);
    ImGui::SetWindowPos(ImVec2(window_width / 3 , window_height /2 - 100));
    static ImGuiTextFilter filter;
    
    filter.Draw("",ImGui::GetWindowWidth());
    ImGui::BeginChild("",ImVec2(ImGui::GetWindowWidth(),200));
    
	const std::vector<Action*>& m_actions = ScenarioManager::getCurrentScenario()->getActions();

	for (size_t i = 0; i < m_actions.size(); ++i) {
		Action *act = m_actions[i];
		if (filter.PassFilter(act->getName().c_str())) {
			if (ImGui::Button(act->getName().c_str(), ImVec2(ImGui::GetWindowWidth()-10,20))) {
				act->act();
				this->m_show = false;
			}
		}
	}		

    style.ScrollBarWidth = 16.0f;

    ImGui::EndChild();
    ImGui::End();
}

static void act_pickseeds_cb(void *) {
   /* if (progStatus != PS_PICKING_SEED) {
		if (!regionGrowing) {
			delete regionGrowing;
		}
		regionGrowing = new RegionGrowing;
        progStatus = PS_PICKING_SEED;
	} else {
        progStatus = PS_NORMAL;
		pickingFace->setColor(ColorManager::COLOR_NORMAL_FACE);
		pickingFace = nullptr;
	}*/
}

static void act_quit_cb(void *) {
//    glfwSetWindowShouldClose(window, true);
}

static void act_genseeds_cb(void *) {
    // glfwSetWindowShouldClose(window, true);
}

static void act_regiongrowing_cb(void*) {
	
}
