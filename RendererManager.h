#ifndef RENDERERMANAGER_H_
#define RENDERERMANAGER_H_

#include <vector>

class Renderer;

class RendererManager {
public:
	static void init();
	static void terminate();
    static void render();
    static void addRenderer(Renderer *renderer) {
        m_renderers.push_back(renderer);
    }
    
    private:
    static std::vector<Renderer*> m_renderers;
};




#endif