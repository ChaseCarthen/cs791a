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
	bool addMask(std::string filename);
	bool addRaster(std::string filename);
	bool createMesh(glm::vec3 color,float wd=1,float ht=1);
	void render(glm::mat4& projection,glm::mat4& view,GLuint& texture);
	void update();
	vector<std::vector<float> > getData();
	void setHeight(float val);
	void setWidth(float val);
	void setLength(float val);
	float getWidth();
	float getHeight();
	float getWorldWidth();
	float getWorldHeight();
	float getXRes();
	float getYRes();
	float getStartX();
	float getStartY();
	float setMaxH(float m);
    void cleanup();
    glm::vec2 getWorldPoint(int i, int j);
    vector<glm::vec2> getNearestWorldPoint(glm::vec2 point);
    void getNearestIndex(glm::vec2 point, float& i, float& j);
    void setTransformation(OGRSpatialReference& a);
    OGRSpatialReference& getSpatialReference()
    {
    	return sr;
    }
    void translate(glm::vec3 position);
    float getXScale();
    float getYScale();
    float getMin();
    float getMax();
    void setMin(float& m);
    void setMax(float& m);
    void setactive(int i , int j, bool val);
    float interpolateHeight(glm::vec2 wp);
    void selectTexture(int i,int bn);
    void setactive(bool val)
   	{
   		selected = val;
   	};
   	int numRasters()
   	{
   		return rasterfs.size();
   	};
private:
	float height,width,length,xres,yres,startx,starty,h,w,th,tw,max,min,mh;
	vector<vector<float>> vecs;
	vector<vector<bool>> active;
	map<string,GLuint> rasters;
	GLuint selectedMask;
	GLuint selectedRaster;
	vector<string> rasterfs;
	vector<string> masks;
	bool selected;
	int bandnum;
	int raster;
	int mask;
	renderer rend;
	GLuint vbo_geometry;
	GLuint elements;
	glm::mat4 model;
	int veccount;
	GDALDataset* ds;
	OGRCoordinateTransformation* transform;
	OGRSpatialReference sr;
};


#endif