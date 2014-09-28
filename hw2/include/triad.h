#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <vector>
#include <includes.h>

#ifndef _TRIAD_H_
#define _TRIAD_H_
typedef CGAL::Exact_predicates_inexact_constructions_kernel            Kernel;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned int, Kernel> Vb;
typedef CGAL::Triangulation_data_structure_2<Vb>                       Tds;
typedef CGAL::Delaunay_triangulation_2<Kernel, Tds>                    Delaunay;
typedef Kernel::Point_2                                                Point;
std::vector<int> triad(std::vector<Point>& points2)
{
  std::vector< std::pair<Point,unsigned int >> ps;
  std::vector<int> its;
  for(int i = 0; i < points2.size(); i++)
  {
    ps.push_back(std::make_pair(points2[i],ps.size()));
  }
  Delaunay triangulation;
  triangulation.insert(ps.begin(),ps.end());

  for(Delaunay::Finite_faces_iterator fit = triangulation.finite_faces_begin();
      fit != triangulation.finite_faces_end(); ++fit) {

    Delaunay::Face_handle face = fit;
    //std::cout << "Triangle:\t" << triangulation.triangle(face) << std::endl;
    //std::cout << "Vertex 0:\t" << triangulation.triangle(face)[0] << std::endl;
    //std::cout << "Vertex 0:\t" << face->vertex(0)->info() << std::endl;
    its.push_back(face->vertex(0)->info());
    its.push_back(face->vertex(1)->info());
    its.push_back(face->vertex(2)->info());
  }
  return its;
};

#endif
