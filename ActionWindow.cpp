#include <iostream>
#include "GUI.h"
#include "ActionWindow.h"

#include "imgui/imgui.h"
#include "MeshLib_Core/Face.h"
#include "growing/growing.h"

extern bool mousePressed[2];
static GLFWwindow *window;
extern ProgStatus progStatus;
extern Face *pickingFace;
extern RegionGrowing *regionGrowing;

ActionWindow::ActionWindow(GLFWwindow *window)
: m_parent(window), m_show(false)
{
	window = GUI::getWindow()
    buildActions();
}

ActionWindow::~ActionWindow() {
    for (auto p : m_actions) {
        delete p;
    }
    m_actions.clear();
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
    for (size_t i = 0; i < m_actions.size(); ++i) {
        Action *act = m_actions[i];
        if (filter.PassFilter(act->buildActionName().c_str())) {
           // if (act->actStatus == IDLING || act->actStatus == PRESSED) {
			
                act->isSelected = ImGui::Button(act->buildActionName().c_str(), ImVec2(ImGui::GetWindowWidth()-10,20));
                if (act->isSelected) {
                    act->transferStatus();
                    act->action_cb(nullptr);
                    this->m_show = false;
                }
        }
    }
    style.ScrollBarWidth = 16.0f;

    ImGui::EndChild();
    ImGui::End();
}

static void act_pickseeds_cb(void *) {
    if (progStatus != PS_PICKING_SEED) {
		if (!regionGrowing) {
			delete regionGrowing;
		}
		regionGrowing = new RegionGrowing;
        progStatus = PS_PICKING_SEED;
	} else {
        progStatus = PS_NORMAL;
		pickingFace->setColor(ColorManager::COLOR_NORMAL_FACE);
		pickingFace = nullptr;
	}
}

static void act_quit_cb(void *) {
    glfwSetWindowShouldClose(window, true);
}

static void act_genseeds_cb(void *) {
    // glfwSetWindowShouldClose(window, true);
}

static void act_regiongrowing_cb(void*) {
	
}

void ActionWindow::buildActions() {
    Action *actGenSeeds = new Action("Generate Seeds");
    actGenSeeds->action_cb = act_genseeds_cb;
    m_actions.push_back(actGenSeeds);

    Action *actPickSeeds = new Action("Picking Seeds", TOGGLED_OFF);
    actPickSeeds->action_cb = act_pickseeds_cb;
    m_actions.push_back(actPickSeeds);

	Action *actRegionGrowing = new Action("Region Growing", TOGGLED_OFF);
	actRegionGrowing->action_cb = act_regiongrowing_cb;
	m_actions.push_back(actRegionGrowing);

    Action *actQuit = new Action("Quit");
    actQuit->action_cb = act_quit_cb;
    m_actions.push_back(actQuit);
}
