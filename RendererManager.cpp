#include "RendererManager.h"
#include "MeshData.h"

#include "Renderer.h"

 std::vector<Renderer*> RendererManager::m_renderers;

void RendererManager::init() {

}

void RendererManager::terminate() {

}

void RendererManager::render() {
    for (auto r : m_renderers) {
        r->render();
    }
}
