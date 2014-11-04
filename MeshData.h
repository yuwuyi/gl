#ifndef MeshData_h__
#define MeshData_h__

#include "MeshLib_Core/Point.h"
#include "kdtree/kdtree.h"

class Mesh;
class Face;
class Vertex;

class MeshData {
public:
	MeshData(Mesh *mesh);
	~MeshData();

	void buildKDTree();
	Face *locate(Vertex *vertex, const Point& point);
private:
	Mesh *m_mesh;
	struct kdtree* ptree;
	struct kdres *presults;

};
#endif // MeshData_h__
