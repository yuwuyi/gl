#ifndef MESHDATAMANAGER_H_
#define MESHDATAMANAGER_H_

#include <vector>

class MeshData;

class MeshDataManager {
public:
    static void init();
    static void terminate();
    
    static void addMeshData(MeshData* m);
    static MeshData* getCurrentMeshData();
    static void setCurrent(MeshData *m);
private:
    static MeshData* m_currentMeshData;
    static std::vector<MeshData*> m_meshdata;
};

#endif