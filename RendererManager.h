#ifndef RENDERERMANAGER_H_
#define RENDERERMANAGER_H_

#include <vector>

class Renderer;
class MeshData;

class RendererManager {
public:
	static void init();
	static void terminate();

private:
    static std::vector<Renderer*> m_renderers;
};





#endif