#include <triangle.h>

triangle::triangle(glm::vec2 A, glm::vec2 B, glm::vec2 C)
{
  a = A;
  b = B;
  c = C;
}

float triangle::getArea()
{
   auto side1 = a - b;
   auto side2 = a - c;
   auto side3 = b - c;
   //cout << std::setprecision(9) << a.x << " " << a.y << " " << b.x << " "<< b.y << " " << c.x << " " << c.y << endl;
   //cout << side1.x << " " << side1.y << endl;
   //cout << std::setprecision(9) << glm::length(side1) << " " << glm::length(side2) << glm::length(side3) << endl;
   float hp = (glm::length(side1)+glm::length(side2)+glm::length(side3))/2;
   // herons formula
   return sqrt(hp*(hp-glm::length(side1))*(hp-glm::length(side2))*(hp-glm::length(side3))); 
}
