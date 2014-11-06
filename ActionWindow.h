#ifndef ACTIONWINDOW_H_
#define ACTIONWINDOW_H_

#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include "Action.h"

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