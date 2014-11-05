#include "GUI.h"
#include "ActionWindow.h"

#include "imgui/imgui.h"

#include "MeshLib_Core/Face.h"
#include "MeshLib_Core/Vertex.h"

#include "growing/growing.h"
#include "kdtree/kdtree.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>




#include <iostream>



//////////////////////////////////////////////////////////////////////////
// Handlers
//////////////////////////////////////////////////////////////////////////

float GUI::translate_x = 0;
float GUI::translate_y = 0;
float GUI::diameter = 0;
glm::vec2 GUI::center = glm::vec2(0, 0);

//static bool isPicking = false;
//extern float translate_x, translate_y;
extern ActionWindow *actWindow;
extern bool mousePressed[2];
extern ProgStatus progStatus;
extern Face *pickingFace;
extern RegionGrowing *regionGrowing;


GLFWwindow * GUI::window = nullptr;



void GUI::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS && button >= 0 && button < 2)
		mousePressed[button] = true;

	if (actWindow->isShown()) {
		return;
	}
//	double xpos = 0, ypos = 0;
//	glfwGetCursorPos(window, &xpos, &ypos);
//
//	if(progStatus == PS_PICKING_SEED && !ImGui::IsMouseHoveringAnyWindow() && button==GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
//	{
//		//TODO: prevent pickingFace multi-entry
//		//TODO: remove pickingFace if getting wrong
//		if (pickingFace) {
//			pickingFace->setColor(ColorManager::COLOR_PATCH_CENTER_FACE);
//			pickingFace->lock();
//			regionGrowing->initPatches(pickingFace);
//			pickingFace = nullptr;
//		}
//
//	}


}

static void glfw_mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
	if (actWindow->isShown()) {
		return;
	}
	//const float window_width = ImGui::GetIO().DisplaySize.x;

	//const float window_height = ImGui::GetIO().DisplaySize.y;


	//if(progStatus == PS_PICKING_SEED && !ImGui::IsMouseHoveringAnyWindow())
	//{
	//	double xx, yy;

	//	xx = -(window_width/window_height * diameter) + xpos/window_width * 2 * (window_width / window_height * diameter);
	//	yy = ( diameter) - ypos/window_height * 2 * diameter;

	//	double pos[3] = {xx + center.x - translate_x, yy  + center.y - translate_y, 0};
	//	presults = kd_nearest(ptree, pos);

	//	if (!presults) {
	//		return;
	//	}
	//	Vertex *v = (Vertex*)kd_res_item( presults, pos );

	//	//Point Location
	//	Face *prevPickingFace = pickingFace;
	//	pickingFace = locate(v, Point(xx + center.x,  yy  + center.y, 0));
	//	if (pickingFace != prevPickingFace) {
	//		if (pickingFace) {
	//			pickingFace->setColor(ColorManager::COLOR_PICKED_FACE);
	//		}
	//		if (prevPickingFace) {
	//			prevPickingFace->setColor(ColorManager::COLOR_NORMAL_FACE);
	//		}

	//	}
	//}

}

void GUI::glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel = (yoffset != 0.0f) ? yoffset > 0.0f ? 1 : - 1 : 0;           // Mouse wheel: -1,0,+1


	if (actWindow->isShown()) {
		return;
	}

	if (ImGui::IsMouseHoveringAnyWindow()) {
		return;
	}

	if (io.MouseWheel == -1) {
		//regionGrowing->grow();

		diameter *= 1.1;

	} else if (io.MouseWheel == 1) {
		diameter *= 0.9;
	}

}

void GUI::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	io.KeyCtrl = (mods & GLFW_MOD_CONTROL) != 0;
	io.KeyShift = (mods & GLFW_MOD_SHIFT) != 0;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		actWindow->setShow(!actWindow->isShown());
	}

	if (ImGui::IsMouseHoveringAnyWindow()) {
		return;
	}


	if (actWindow->isShown()) {
		return;
	}

	int factor = 1;
	if (mods & GLFW_MOD_ALT) {
		factor = 5;
	}

	if (key == GLFW_KEY_W) {
		translate_y -= diameter / 100 * factor;
	} else if (key == GLFW_KEY_S) {
		translate_y += diameter / 100 * factor;
	} else if (key == GLFW_KEY_A) {
		translate_x -= diameter / 100 * factor;
	} else if (key == GLFW_KEY_D) {
		translate_x += diameter / 100 * factor;
	}
}

static void glfw_char_callback(GLFWwindow* window, unsigned int c)
{
	if (c > 0 && c < 0x10000)
		ImGui::GetIO().AddInputCharacter((unsigned short)c);
}

//
//static void glfw_mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
//    
//}


static GLFWwindow *window = nullptr;

void GUI::init() {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(1080, 768, "Simple example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetCursorPosCallback(window, glfw_mouse_move_callback);
	glfwSetScrollCallback(window, glfw_scroll_callback);
	glfwSetCharCallback(window, glfw_char_callback);


	//initial glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << glGetError() << "\n";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	err = glGetError(); //this is to ignore INVALID ENUM error 1282
}

GLFWwindow* GUI::getWindow() {
	return window;
}

void GUI::terminate() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

glm::mat4 GUI::getMVP() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 P = glm::mat4(1);
	glm::mat4 MV = glm::translate(glm::mat4(1.0f),glm::vec3(-center.x, -center.y, 0.0));
	MV = glm::translate(MV,glm::vec3(translate_x, translate_y, 0.0));
	float aspect = (float)width / (float)height;
	P = glm::ortho(-aspect * diameter, aspect * diameter, -diameter, diameter, -1.0f, 1.0f);
	return P*MV;
}


glm::mat4 GUI::getMVP_inv() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 P = glm::mat4(1);
	glm::mat4 MV = glm::translate(glm::mat4(1.0f),glm::vec3(-center.x, -center.y, 0.0));
	MV = glm::translate(MV,glm::vec3(translate_x, translate_y, 0.0));
	float aspect = (float)width / (float)height;
	P = glm::ortho(-aspect * diameter, aspect * diameter, -diameter, diameter, -1.0f, 1.0f);
	
	return glm::inverse(P*MV);
}

