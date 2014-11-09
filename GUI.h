#ifndef GUI_H_
#define GUI_H_
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "MeshLib_Core/Point.h"

#include <glm/glm.hpp>

enum ProgStatus {
    PS_NORMAL,
    PS_PICKING_SEED,
	PS_REGION_GROWING,
};



class GUI {
public:
	static void init();
	static void terminate();
	static GLFWwindow* getWindow();
    
    static void setDiameter(double d) {
        diameter = d;
    }
	static void setCenter(const glm::vec2& c) {
		center = c;
	}
    static glm::mat4 getMVP();
	static glm::mat4 getMVP_inv();

    static glm::vec2 screen2world(double x, double y);
private:
	static GLFWwindow *window;

	//handlers
	//static void glfw_mouse_move_callback(GLFWwindow* window, double xpos, double ypos);

    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static float translate_x, translate_y;
    static float diameter;
	static glm::vec2 center;

};

#endif