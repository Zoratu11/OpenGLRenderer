//Inspired by the learnopengl class for shaders

#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
	unsigned int ID;

	Shader();
	Shader(const std::string& vPath, const std::string& fPath);
	~Shader();

	void loadShaders(const std::string& vPath, const std::string& fPath);

	void use();

	//Setter functions for uniform variables in the shaders
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value)	const;
	void setFloat(const std::string& name, float value)	const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;
};