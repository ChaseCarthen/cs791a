#include <renderer.h>

renderer::renderer()
{
	 shaders = std::vector<shader> ();
}

void renderer::init()
{
	program = glCreateProgram();
}
bool renderer::addShader(GLuint& Shader,string fname)
{
	bool status = false;
	shader s(Shader);
	status = s.open(fname);
	cout << "SHADER VATE: "<< Shader << endl;
	if(status)
	{
		shaders.push_back(s);
	}
	return status;
}

bool renderer::link()
{
	bool status = false;
	for(int i=0; i < shaders.size();  i++)
	{

		cout << "LINKING: "<< shaders[i].getSource() << endl;
		status = shaders[i].link(program);
		if(!status)
		{
			break;
		}
	}
	return status;
}

bool renderer::compile()
{
	bool status = false;
	for(int i=0; i < shaders.size();  i++)
	{
		cout << "COMPILING: "<< shaders[i].getSource() << endl;
		status = shaders[i].compile();
		if(!status)
		{
			break;
		}
	}
	return status;
}

void renderer::cleanup()
{
	for(auto i : shaders)
	{
		i.cleanup();
	}
	glDeleteProgram(program);
	shaders.clear();
}

bool renderer::getAttribute(string str, GLint& val)
{
	val = glGetAttribLocation(program,str.c_str());
	return val != -1;
}

bool renderer::getUniform(string str, GLint& val)
{
	val = glGetUniformLocation(program,str.c_str());
	return val != -1;
}

void renderer::useProgram()
{
	glUseProgram(program);
}

void renderer::render(int count)
{
 glDrawElements(
     GL_TRIANGLES,      // mode
     count,    // count
     GL_UNSIGNED_INT,   // type
     (void*)0           // element array buffer offset
 );
} 