#include "ShaderManager.h"

#include <iostream>

std::map<std::string, GLSLShader> ShaderManager::m_shader;


void ShaderManager::init() {
	GLSLShader colorShader;
	colorShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/colorshader.vert");
	colorShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/colorshader.frag");

	//compile and link shader
	colorShader.CreateAndLinkProgram();
	colorShader.Use();
	//add attributes and uniforms
	colorShader.AddAttribute("vVertex");
	colorShader.AddAttribute("vColor");
	colorShader.AddAttribute("vUV");
	colorShader.AddUniform("MVP");
	colorShader.AddUniform("textureMap");
	glUniform1i(colorShader("textureMap"),0);
	colorShader.UnUse();
	m_shader["Color"] = colorShader;


	GLSLShader textureShader;

	textureShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/textureshader.vert");
	textureShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/textureshader.frag");

	//compile and link shader
	textureShader.CreateAndLinkProgram();
	textureShader.Use();
	//add attributes and uniforms
	textureShader.AddAttribute("vVertex");
	textureShader.AddAttribute("vColor");
	textureShader.AddAttribute("vUV");
	textureShader.AddUniform("MVP");
	textureShader.AddUniform("textureMap");
	glUniform1i(textureShader("textureMap"),0);
	textureShader.UnUse();
	m_shader["Texture"] = textureShader;

	GLSLShader wireframeShader;

	wireframeShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/wireframeshader.vert");
	wireframeShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/wireframeshader.geom");
	wireframeShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/wireframeshader.frag");

	//compile and link shader
	wireframeShader.CreateAndLinkProgram();
	wireframeShader.Use();
	//add attributes and uniforms
	wireframeShader.AddAttribute("vVertex");
	wireframeShader.AddAttribute("vColor");
	wireframeShader.AddAttribute("vUV");
	wireframeShader.AddUniform("MVP");
	wireframeShader.AddUniform("textureMap");
	glUniform1i(wireframeShader("textureMap"),0);
	wireframeShader.UnUse();

	m_shader["WireFrame"] = wireframeShader;
}

void ShaderManager::terminate() {
	for (auto s : m_shader) {
		s.second.DeleteShaderProgram();
	}

}

GLSLShader ShaderManager::getShader(const std::string& shaderName) {
	auto iter = m_shader.find(shaderName);
	if (iter == m_shader.end()) {
		std::cerr << "No such shader!";
		exit(-1);
	}

	return (*iter).second;
}