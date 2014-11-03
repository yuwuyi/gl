#ifndef ACTIONWINDOW_H_
#define ACTIONWINDOW_H_

#include <GLFW/glfw3.h>
#include <vector>
#include <string>


enum ActionStatus {
    IDLING,
    TOGGLED_OFF,
    TOGGLED_ON,
    PRESSED,
};


struct Action {
    Action(const std::string name, ActionStatus as= IDLING)
            : actName(name), actStatus(as), isSelected(false)
    {

    }

    void transferStatus() {
        if (actStatus == TOGGLED_OFF) {
            actStatus = TOGGLED_ON;
        } else if (actStatus == TOGGLED_ON) {
            actStatus = TOGGLED_OFF;
        } else if (actStatus == IDLING) {
            actStatus = PRESSED;
        }
    }

    std::string buildActionName() {
        if (actStatus == TOGGLED_OFF) {
            return actName + ":      OFF";
        } else if (actStatus == TOGGLED_ON) {
            return actName + ":      ON";
        }
        return actName;
    }

    void (*action_cb)(void *);
    ActionStatus actStatus;
    std::string actName;
    bool isSelected;
};



class ActionWindow {
public:
    ActionWindow(GLFWwindow *parent);
    ~ActionWindow();
    void render();
    void buildActions();

public:
    bool isShown() const {
        return m_show;
    }

    void setShow(bool m_show) {
        this->m_show = m_show;
    }




private:
    GLFWwindow *m_parent;
    bool m_show;
    std::vector<Action*> m_actions;
};

#endif