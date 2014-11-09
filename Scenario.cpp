#include "Scenario.h"
#include "Action.h"

#include "Renderer.h"

#include "imgui/imgui.h"

#include "GUI.h"

Scenario::Scenario() {
}

Scenario::~Scenario() {
	for (auto p : m_actions) {
		delete p;
	}
	m_actions.clear();

}

NormalScenario::NormalScenario() {
	buildActions();
}

NormalScenario::~NormalScenario() {
    
}


void NormalScenario::buildActions() {
	m_actions.push_back(new PickingAction("Picking"));
}


void NormalScenario::render() {
}

PickScenario::PickScenario() {

}

PickScenario::~PickScenario() {
}

void PickScenario::render() {

}


void PickScenario::mousePressEvent() {
    
}

void PickScenario::mouseMoveEvent(GLFWwindow* window, int button, int action, int mods) {
    const float window_width = ImGui::GetIO().DisplaySize.x;
    
    const float window_height = ImGui::GetIO().DisplaySize.y;
    
    	double xpos = 0, ypos = 0;
    	glfwGetCursorPos(window, &xpos, &ypos);
    

//    	if(!ImGui::IsMouseHoveringAnyWindow())
//        	{
////        		double xx, yy;
//        
//                glm::vec2 world_pos = GUI::screen2world(xpos, ypos);
////        		xx = -(window_width/window_height * diameter) + xpos/window_width * 2 * (window_width / window_height * diameter);
////        		yy = ( diameter) - ypos/window_height * 2 * diameter;
//        
////        		double pos[3] = {xx + center.x - translate_x, yy  + center.y - translate_y, 0};
//                double pos[3] = {world_pos[0], world_pos[1], 0};
//                presults = kd_nearest(ptree, pos);
//        
//        		if (!presults) {
//        			return;
//        		}
//        		Vertex *v = (Vertex*)kd_res_item( presults, pos );
//        
//        		//Point Location
//        		Face *prevPickingFace = pickingFace;
//        		pickingFace = locate(v, Point(xx + center.x,  yy  + center.y, 0));
//        		if (pickingFace != prevPickingFace) {
//        			if (pickingFace) {
//        				pickingFace->setColor(ColorManager::COLOR_PICKED_FACE);
//        			}
//        			if (prevPickingFace) {
//        				prevPickingFace->setColor(ColorManager::COLOR_NORMAL_FACE);
//        			}
//        
//        		}
//        	}
}