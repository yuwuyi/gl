#include "Renderer.h"

#include "MeshData.h"
#include "MeshLib_Core/Mesh.h"
#include "MeshLib_Core/Iterators.h"

#include "ShaderManager.h"
#include "ColorManager.h"

#include "GUI.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


void MeshRenderer::loadData(MeshData* meshdata) {

	m_currentMeshData = meshdata;

	Mesh *mesh = meshdata->getMesh();

	GLSLShader colorShader = ShaderManager::getShader("Color");


	std::vector<glm::vec2> pos;
	std::vector<int> indices;
	std::vector<glm::vec4> colors;

	
	for (MeshFaceIterator mfit(mesh); !mfit.end(); ++mfit) {
		Face *f = *mfit;
		f->setColor(ColorManager::COLOR_NORMAL_FACE);
		for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit) {

			pos.push_back(glm::vec2((*fvit)->point()[0], (*fvit)->point()[1]));
			colors.push_back(glm::vec4(ColorManager::COLOR_NORMAL_FACE[0], ColorManager::COLOR_NORMAL_FACE[1], ColorManager::COLOR_NORMAL_FACE[2], 1.0f));
		}
	}
	triangle_num = pos.size();

	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);
	glGenBuffers(1, &vboColorID);


	GLsizei stride = sizeof(glm::vec2);
	glBindVertexArray(vaoID);

	glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
	//pass triangle verteices to buffer object

	glBufferData (GL_ARRAY_BUFFER, sizeof(glm::vec2) * pos.size(), &pos[0], GL_STATIC_DRAW);
	//enable vertex attribute array for position
	glEnableVertexAttribArray(colorShader["vVertex"]);
	glVertexAttribPointer(colorShader["vVertex"], 2, GL_FLOAT, GL_FALSE,stride,0);

	glBindBuffer (GL_ARRAY_BUFFER, vboColorID);
	glBufferData (GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorShader["vColor"]);
	glVertexAttribPointer(colorShader["vColor"], 4, GL_FLOAT, GL_FALSE,4 * sizeof(GLfloat), 0);



//update

	meshdata->vaoID = vaoID;
	meshdata->vboColorID = vboColorID;
	meshdata->vboVerticesID = vboVerticesID;
	meshdata->triangle_num = triangle_num;

}

void MeshRenderer::render() {
	//update the color..
	Mesh *mesh = m_currentMeshData->getMesh();

	GLSLShader colorShader = ShaderManager::getShader("Color");
	GLSLShader wireframeShader = ShaderManager::getShader("WireFrame");


	std::vector<glm::vec4> colors;
	for (MeshFaceIterator mfit(mesh); !mfit.end(); ++mfit) {
		Face *f = *mfit;
		for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit) {
			Point fcol = f->getColor();
			colors.push_back(glm::vec4(fcol[0], fcol[1], fcol[2], 1.0f));
		}
	}

	glBindBuffer (GL_ARRAY_BUFFER, vboColorID);
	glBufferData (GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorShader["vColor"]);
	glVertexAttribPointer(colorShader["vColor"], 4, GL_FLOAT, GL_FALSE,4 * sizeof(GLfloat), 0);




	glEnable(GL_BLEND);

	//    glm::mat4 T		= glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -50));
	//
	//    glm::mat4 Rx	= glm::rotate(T,  rotation_x, glm::vec3(1.0f, 0.0f, 0.0f));
	//    glm::mat4 Ry	= glm::rotate(Rx, rotation_y, glm::vec3(0.0f, 1.0f, 0.0f));
	//    glm::mat4 MV	= glm::rotate(Ry, rotation_z, glm::vec3(0.0f, 0.0f, 1.0f));

	/*const float width  = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
*/

	colorShader.Use();
	glUniformMatrix4fv(colorShader("MVP"), 1, GL_FALSE, glm::value_ptr(GUI::getMVP()));
	
	glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, triangle_num);
	glBindVertexArray(0); //unbind
	colorShader.UnUse();



	wireframeShader.Use();
	glUniformMatrix4fv(wireframeShader("MVP"), 1, GL_FALSE, glm::value_ptr(GUI::getMVP()));
	glBindVertexArray(vaoID);
	//    glDrawElements(GL_TRIANGLES, triangle_num, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, triangle_num);

	glBindVertexArray(0); //unbind
	wireframeShader.UnUse();

	glDisable(GL_BLEND);

}