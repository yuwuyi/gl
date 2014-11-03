#ifndef ShaderManager_h__
#define ShaderManager_h__


#include "GLSLShader.h"

#include <string>
#include <map>

class ShaderManager {
public:
	static void init();
	static void terminate();
	static GLSLShader getShader(const std::string& shaderName);
private:
	static std::map<std::string, GLSLShader> m_shader;
};
#endif // ShaderManager_h__
