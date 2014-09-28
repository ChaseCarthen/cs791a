#include <includes.h>
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
#include <renderer.h>
#ifndef _GISHANDLER_H_
#define _GISHANDLER_H_
using namespace std;
struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 uv;
    bool keep;
};

bool getRawValuesFromFile(string fname,vector<vector<float>>& vecs);

int fromRawCreateMesh(vector<vector<float>>& input,float min, float max,float width, float height,GLuint& vbo_geometry,GLuint& elements,glm::vec3 color,vector<vector<bool>> activeness);

void bilinearintertop(vector<vector<float>>& input, int width, int height);

void  generateTexture(string fname, GLuint& tex, int bandnum);


#endif
