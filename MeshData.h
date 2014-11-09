#ifndef MeshData_h__
#define MeshData_h__

#include "GL/glew.h"
#include "MeshLib_Core/Point.h"
#include "kdtree/kdtree.h"
#include <glm/glm.hpp>

class Mesh;
class Face;
class Vertex;

class MeshData {
public:
	MeshData(Mesh *mesh);
	~MeshData();

	Mesh *getMesh() { return m_mesh; }
	void buildKDTree();
	Face *locate(Vertex *vertex, const Point& point);
    void calculateCenter();
	GLuint vaoID, vboColorID;
	GLuint vboVerticesID;
	GLint triangle_num;
	float getDiameter() const { return diameter; }
	const glm::vec2& getCenter() const { return center; }

private:
	Mesh *m_mesh;
	struct kdtree* ptree;
	struct kdres *presults;
    glm::vec2 center;
    float  diameter;
};
#endif // MeshData_h__
