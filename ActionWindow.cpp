#include <iostream>
#include "GUI.h"
#include "ActionWindow.h"

#include "imgui/imgui.h"

extern bool mousePressed[2];
extern GLFWwindow *window;
extern ProgStatus progStatus;

ActionWindow::ActionWindow(GLFWwindow *window)
: m_parent(window), m_show(false)
{

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
    const ImGuiWindowFlags layout_flags = 0;//ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar  | ImGuiWindowFlags_NoMove
//            | ImGuiWindowFlags_NoResize;

    ImGui::Begin("Action Window", &m_show, ImVec2(window_width / 3,200), fill_alpha, layout_flags);
    ImGui::SetWindowPos(ImVec2(window_width / 3 , window_height /2 - 100));
    static ImGuiTextFilter filter;
    //ImGui::SetKeyboardFocusHere();
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
    if (progStatus == PS_NORMAL)
        progStatus = PS_PICKING_SEED;
    else
        progStatus = PS_NORMAL;
}

static void act_quit_cb(void *) {
    glfwSetWindowShouldClose(window, true);
}

static void act_genseeds_cb(void *) {
    // glfwSetWindowShouldClose(window, true);
}

void ActionWindow::buildActions() {
    Action *actGenSeeds = new Action("Generate Seeds");
    actGenSeeds->action_cb = act_genseeds_cb;
    m_actions.push_back(actGenSeeds);

    Action *actPickSeeds = new Action("Picking Seeds", TOGGLED_OFF);
    actPickSeeds->action_cb = act_pickseeds_cb;
    m_actions.push_back(actPickSeeds);

    Action *actQuit = new Action("Quit");
    actQuit->action_cb = act_quit_cb;
    m_actions.push_back(actQuit);

}
