#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif
//#include <GL/glut.h> 

// Class for a shader loader
class shader
{
   public:
      shader()
      {
      };
      shader(GLuint);
      bool open(std::string);
      bool compile();
      bool link(GLuint program);
   private:
     GLuint source;
};
