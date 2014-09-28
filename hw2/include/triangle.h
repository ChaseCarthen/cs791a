#include <includes.h>
#include <renderer.h>

#include <vector>


using namespace std;

#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
class triangle
{
public:
	triangle(glm::vec2 A, glm::vec2 B, glm::vec2 C);
	float getArea();
private:
	glm::vec2 a,b,c;
};


#endif
