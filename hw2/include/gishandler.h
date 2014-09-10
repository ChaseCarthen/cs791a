#include <gdal_priv.h>
#include <cpl_conv.h>
#include <vector>
#include <string>
#include <iostream>
// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
//#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#ifndef _GISHANDLER_H_
#define _GISHANDLER_H_
using namespace std;
struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

vector<vector<float>> getRawValuesFromFile(string fname);

int fromRawCreateMesh(vector<vector<float>>& input,float min, float max,GLuint& vbo_geometry,GLuint& elements); 
#endif
