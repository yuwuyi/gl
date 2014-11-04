#ifndef ModeManager_h__
#define ModeManager_h__

#include "GLFW/glfw3.h"
#include <vector>

class ProgMode;



class ModeManager {
public:
	static void init();
	static void terminate();
	static ProgMode* getCurrentMode() { return m_current; }

private:
	static ProgMode *m_current;
	static std::vector<ProgMode*> m_progmodes;
};

class ProgMode {
public:
	virtual void MouseClickEvent(GLFWwindow* window, int button, int action, int mods) = 0;
	virtual void MouseMoveEvent(GLFWwindow* window, int button, int action, int mods) = 0;

};

class NormalProgMode : public ProgMode {
public:
	virtual void MouseClickEvent(GLFWwindow* window, int button, int action, int mods);
	virtual void MouseMoveEvent(GLFWwindow* window, int button, int action, int mods);
};

class PickProgMode : public ProgMode {
public:
	virtual void MouseClickEvent(GLFWwindow* window, int button, int action, int mods);
	virtual void MouseMoveEvent(GLFWwindow* window, int button, int action, int mods);
};


#endif // ModeManager_h__
