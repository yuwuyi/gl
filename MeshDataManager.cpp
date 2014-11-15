#include "MeshDataManager.h"
#include "MeshData.h"

MeshData * MeshDataManager::m_currentMeshData = nullptr;
std::vector<MeshData*> MeshDataManager::m_meshdata;

void MeshDataManager::addMeshData(MeshData* m) {
    m_meshdata.push_back(m);
}

MeshData* MeshDataManager::getCurrentMeshData() {
    return m_currentMeshData;
}

void MeshDataManager::setCurrent(MeshData *m) {
    m_currentMeshData = m;
}

void MeshDataManager::terminate() {
    for (auto md : m_meshdata) {
        delete md;
    }
}