#include <includes.h>
#include <vector>
#include <renderer.h>
#include <terrain.h>
using namespace std;

#ifndef _SHAPE_H_
#define _SHAPE_H_
class shape
{
public:
	shape();
	bool load(string filename);
	bool createMesh(OGRSpatialReference* sr2,glm::vec2 origin,glm::vec2 scale,terrain& t);
	void render(glm::mat4& projection,glm::mat4& view);

private:
	vector<vector<glm::vec2>> points;
	OGRDataSource* ds;
	OGRSpatialReference* sr;
	renderer rend;
	GLuint vbo_geometry;
	GLuint elements;
	glm::mat4 model;
	int veccount;
};
#endif