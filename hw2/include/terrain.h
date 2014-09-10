#include <includes.h>
#include <renderer.h>

#include <vector>


using namespace std;

#ifndef _TERRAIN_H_
#define _TERRAIN_H_
class terrain
{
public:
	terrain();
	bool loadTerrain(std::string filename);
	void render(glm::mat4& projection,glm::mat4& view,GLuint& texture);
	void update();
	void setHeight(float val);
	void setWidth(float val);
	void setLength(float val);
    void cleanup();
private:
	float height,width,length;
	vector<vector<float>> vecs;
	renderer rend;
	GLuint vbo_geometry;
	GLuint elements;
	glm::mat4 model;
	int veccount;
};


#endif