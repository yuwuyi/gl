#include "ModeManager.h"
#include "imgui/imgui.h"


ProgMode *ModeManager::m_current = nullptr;
std::vector<ProgMode*> ModeManager::m_progmodes; 

void NormalProgMode::MouseClickEvent(GLFWwindow* window, int button, int action, int mods) {

}

void NormalProgMode::MouseMoveEvent(GLFWwindow* window, int button, int action, int mods) {

}


void PickProgMode::MouseClickEvent(GLFWwindow* window, int button, int action, int mods) {

}

void PickProgMode::MouseMoveEvent(GLFWwindow* window, int button, int action, int mods) {
	const float window_width = ImGui::GetIO().DisplaySize.x;

	const float window_height = ImGui::GetIO().DisplaySize.y;


	if(!ImGui::IsMouseHoveringAnyWindow())
	{
		double xx, yy;

		xx = -(window_width/window_height * diameter) + xpos/window_width * 2 * (window_width / window_height * diameter);
		yy = ( diameter) - ypos/window_height * 2 * diameter;

		double pos[3] = {xx + center.x - translate_x, yy  + center.y - translate_y, 0};
		presults = kd_nearest(ptree, pos);

		if (!presults) {
			return;
		}
		Vertex *v = (Vertex*)kd_res_item( presults, pos );

		//Point Location
		Face *prevPickingFace = pickingFace;
		pickingFace = locate(v, Point(xx + center.x,  yy  + center.y, 0));
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


void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	const float window_width = ImGui::GetIO().DisplaySize.x;

	const float window_height = ImGui::GetIO().DisplaySize.y;

	if (action == GLFW_PRESS && button >= 0 && button < 2)
		mousePressed[button] = true;

	if (actWindow->isShown()) {
		return;
	}

	double xpos = 0, ypos = 0;
	glfwGetCursorPos(window, &xpos, &ypos);

	if(progStatus == PS_PICKING_SEED && !ImGui::IsMouseHoveringAnyWindow() && button==GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//TODO: prevent pickingFace multi-entry
		//TODO: remove pickingFace if getting wrong
			pickingFace->setColor(ColorManager::COLOR_PATCH_CENTER_FACE);
			pickingFace->lock();
//			regionGrowing->initPatches(pickingFace);
//			pickingFace = nullptr;
	}
}