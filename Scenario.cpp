#include "Scenario.h"
#include "Action.h"

#include "MeshData.h"
#include "MeshDataManager.h"

#include "Renderer.h"

#include "MeshLib_Core/Face.h"
#include "ColorManager.h"

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

PickScenario::PickScenario()
: pickingFace(nullptr), prevPickingFace(nullptr){

}

PickScenario::~PickScenario() {
}

void PickScenario::render() {

}


void PickScenario::mousePressEvent() {
    
}

void PickScenario::mouseMoveEvent(GLFWwindow* window, double xpos, double ypos) {
    	if(!ImGui::IsMouseHoveringAnyWindow())
        	{

                MeshData *meshData = MeshDataManager::getCurrentMeshData();
                glm::vec2 world_pos = GUI::screen2world(xpos, ypos);


                double pos[3] = {world_pos[0], world_pos[1], 0};

        		pickingFace = meshData->locate(pos);

        		if (pickingFace != prevPickingFace) {
        			if (pickingFace) {
        				pickingFace->setColor(ColorManager::COLOR_PICKED_FACE);
        			}
        			if (prevPickingFace) {
        				prevPickingFace->setColor(ColorManager::COLOR_NORMAL_FACE);
        			}
        
        		}
        	}
}