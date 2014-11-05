#ifndef RENDERERMANAGER_H_
#define RENDERERMANAGER_H_

#include <vector>

class Renderer;
class MeshData;

class RendererManager {
public:
private:
    std::vector<Renderer*> m_renderers;
};





#endif