#ifndef Renderer_h__
#define Renderer_h__

#include "GL/glew.h"
class MeshData;
class Renderer {
public:
	virtual void render() = 0;
};

class MeshRenderer : public Renderer {

public:
	void loadData(MeshData* meshdata);
	virtual void render();

private:
	MeshData *m_currentMeshData;
	GLuint vaoID; 
	GLuint vboColorID;
	GLuint vboVerticesID;
	GLint triangle_num;
};

#endif // Renderer_h__
