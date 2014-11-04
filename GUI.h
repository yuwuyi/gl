#ifndef GUI_H_
#define GUI_H_
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "MeshLib_Core/Point.h"

enum ProgStatus {
    PS_NORMAL,
    PS_PICKING_SEED,
	PS_REGION_GROWING,
};

static const char *MODE_STR[] = {"", "Picking", "Region Growing"};

struct ColorManager {
    const static   Point COLOR_PICKED_FACE;
    const static   Point COLOR_NORMAL_FACE;
    const static   Point COLOR_PATCH_CENTER_FACE;
};



class GUI {
public:
	static void init();
	static void terminate();
	static GLFWwindow* getWindow();
private:
	static GLFWwindow *window;

};

#endif