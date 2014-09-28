#include <iostream>
#include <vector>
#include <terrain.h>
#include <string>
#include <includes.h>
#include <queue>
#include <triangle.h>
#include <utility> 
#include <shape.h>

#ifndef _TERRAINMANAGER_H_
#define _TERRAINMANAGER_H_
using namespace std;

class terrainManager
{

public:
terrainManager();
std::vector<std::vector<float> > getData(int index, bool largest=false);
void interpolateMesh(bool all);
void generateGraphMesh(bool all);
void render(glm::mat4& projection,glm::mat4& view,GLuint& texture);
int numberOfTerrains();
string getProjection();
bool addTerrain(string filename);
bool addShape(string filename);
void createMeshes();
void setActiveTerrain();
void selectTerrain(int i);
void selectTexture(int i,int bn);
void addRaster(string filename);
void addMask(string filename);
int getRasterCount()
{
	return terrains[selected].numRasters();
};


private:
vector<terrain> terrains;
vector<shape> shapes;
string projection;
glm::vec3 origin;
float scalex,scaley;
int selected;
};
#endif