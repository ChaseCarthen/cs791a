#include <includes.h>
#include <shader.h>
#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#ifndef _RENDERER_H_
#define _RENDERER_H_
// A Renderer class that will store and hold shader programs
// It will compile and link all shader passed to it
class renderer
{
public:
	renderer();
	void init();
	bool addShader(GLuint& Shader,string fname);
	bool link();
	bool compile();
	void cleanup();
	//Shader Access Functions go here

	// Uniforms
	bool getUniform(string str, GLint& val);
	void setUniformMatrix4x4(string name,glm::mat4 mat)
	{
		GLint v; 
        getUniform(name,v);
		glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(mat));
	};
    void setUniformFloat(string name, float val)
    {
       GLint v; 
       getUniform(name,v);
       glUniform1f(v,val);
    };
	// Attributes
	bool getAttribute(string str, GLint& val);
    void enableVertexAttribPointer(string name)
    {
	GLint val;
	getAttribute(name,val);
	glEnableVertexAttribArray(val);
    };
    void disableVertexAttribPointer(string name)
    {
	GLint val;
	getAttribute(name,val);
	glDisableVertexAttribArray(val);
    };
	void setGLVertexAttribPointer(string name,int elements, GLenum type, bool t,int size,void* offset)
	{
	GLint val;
	getAttribute(name,val);
    glVertexAttribPointer( val,//location of attribute
                           elements,//number of elements
                           type,//type
                           t,//normalized?
                           size,//stride
                           offset);
	};

	// Others for other versions of glsl.

	// Assuming 1D Texture...
	void setTexture(string name,GLuint texture)
	{
		GLint v; 
        getUniform(name,v);
    	glEnable(GL_TEXTURE_1D);
    	glUniform1i(v, 0);
 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_1D, texture);
	};

    // Set this render as the current program
    void useProgram(); 

	//Render Function
	void render(int count);
private:
	std::vector<shader> shaders;
	GLuint program;

};


#endif