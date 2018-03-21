#include <boost/geometry.hpp>
#include <iostream>
#include <list>

const int dimension = 2;
using point = boost::geometry::model::point<int, dimension, boost::geometry::cs::cartesian>;
using box = boost::geometry::model::box<point>;
using boost::geometry::max_corner;
using boost::geometry::min_corner;

int main(int, char **)
{
  std::list<box> result;
  box a(point(0, 0), point(10, 10)), b(point(3, 3), point(7, 7));
  boost::geometry::difference(a,b,result);
  std::cout << "BOX(0 0, 10 10)\\BOX(3 3, 7 7)\n\n";
  for(const auto &r : result)
  {
    std::cout << "BOX( " << boost::geometry::get<min_corner, 0>(r) << " " << boost::geometry::get<min_corner, 1>(r) << " , "
              << boost::geometry::get<max_corner, 0>(r) << " " << boost::geometry::get<max_corner, 1>(r) << " )\n";
  }
  return 0;
}
