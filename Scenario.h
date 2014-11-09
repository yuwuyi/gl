#ifndef Scenario_h__
#define Scenario_h__

#include <vector>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

class Action;
class Renderer;

class Scenario {
public:
	Scenario();
	virtual ~Scenario();
	virtual void render() = 0;
	virtual const std::vector<Action*>& getActions() const {
		return m_actions;
	}

    virtual void mousePressEvent() {}
    virtual void mouseMoveEvent(GLFWwindow* window, int button, int action, int mods) {}
protected:
	std::vector<Action*> m_actions;
    Renderer *m_renderer;
    
};


class NormalScenario : public Scenario {
public:
	NormalScenario();
	virtual ~NormalScenario();
	virtual void render();

private:
	void buildActions();
};


class PickScenario : public Scenario {
public:
	PickScenario();
	virtual ~PickScenario();
	virtual void render();
    
    virtual void mousePressEvent();
    virtual void mouseMoveEvent(GLFWwindow* window, int button, int action, int mods);
};


#endif // Scenario_h__
