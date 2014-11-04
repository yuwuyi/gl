#include "GUI.h"
#include "ActionWindow.h"

#include "imgui/imgui.h"

#include "MeshLib_Core/Face.h"
#include "MeshLib_Core/Vertex.h"

#include "growing/growing.h"
#include "kdtree/kdtree.h"

#include <glm/glm.hpp>


#include <iostream>

const Point ColorManager::COLOR_PICKED_FACE = Point (0, 1, 0);
const Point ColorManager::COLOR_NORMAL_FACE = Point (0, 0.4, 0.7);
const Point ColorManager::COLOR_PATCH_CENTER_FACE = Point(1, 0, 0);


//////////////////////////////////////////////////////////////////////////
// Handlers
//////////////////////////////////////////////////////////////////////////

static bool isPicking = false;
extern float translate_x, translate_y;
extern ActionWindow *actWindow;
extern bool mousePressed[2];
extern ProgStatus progStatus;
extern Face *pickingFace;
extern GLfloat  diameter;
extern RegionGrowing *regionGrowing;
extern glm::vec2 center;
extern struct kdtree* ptree;
extern struct kdres *presults;

GLFWwindow * GUI::window = nullptr;


void barycentric(Face *f, const Point& point, float  (&lambda)[3]) {
	Point points [3] ;
	int i = 0;
	for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit, ++i) {
		points[i] = (*fvit)->point();
	}

	double x = point[0], y = point[1];
	double x1 = points[0][0], y1 = points[0][1];
	double x2 = points[1][0], y2 = points[1][1];
	double x3 = points[2][0], y3 = points[2][1];

	double dimo = ((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
	lambda[0] = ((y2-y3)*(x-x3) + (x3-x2)*(y-y3)) / dimo;
	lambda[1] = ((y3-y1)*(x-x3) + (x1-x3)*(y-y3)) / dimo;
	lambda[2] = 1.0 - lambda[0] - lambda[1];

}

Face *locate(Vertex *vertex, const Point& point) {
	Face *inFace = nullptr;
	for (VertexFaceIterator vfit(vertex); !vfit.end(); ++vfit) {
		inFace = *vfit;
		float lambda[3];
		barycentric(inFace, point, lambda);

		for (int i = 0; i < 3; ++i) {
			if (lambda[i] < 0 || lambda[i] > 1) {
				inFace = nullptr;
				break;
			}
		}

		if (inFace) {
			break;
		}
	}

	return inFace;
}




static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
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
		if (pickingFace) {
			pickingFace->setColor(ColorManager::COLOR_PATCH_CENTER_FACE);
			pickingFace->lock();
			regionGrowing->initPatches(pickingFace);
			pickingFace = nullptr;
		}

	}
}

static void glfw_mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
	if (actWindow->isShown()) {
		return;
	}
	const float window_width = ImGui::GetIO().DisplaySize.x;

	const float window_height = ImGui::GetIO().DisplaySize.y;


	if(progStatus == PS_PICKING_SEED && !ImGui::IsMouseHoveringAnyWindow())
	{

		double xx, yy;

		xx = -(window_width/window_height * diameter) + xpos/window_width * 2 * (window_width / window_height * diameter);
		yy = ( diameter) - ypos/window_height * 2 * diameter;

		//        std::cout << "xx, yy" << xx + center.x + translate_x<< " " << yy  + center.y + translate_y<< "\n";
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

static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
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

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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