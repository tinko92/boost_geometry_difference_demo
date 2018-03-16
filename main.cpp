#include "boxdiff.h"
#include <boost/geometry.hpp>
#include <iostream>
#include <list>

const int dimension = 2;
using point = boost::geometry::model::point<int, dimension, boost::geometry::cs::cartesian>;
using box = boost::geometry::model::box<point>;
const auto policy = boxdiff::slice_policy::slices;
using boost::geometry::max_corner;
using boost::geometry::min_corner;

int main(int, char **)
{
  std::list<box> result;
  box a(point(0, 0), point(5, 5)), b(point(4, 4), point(9, 9));
  boxdiff::box_diff(a, b, result);
  std::cout << "BOX(0 0, 5 5)\\BOX(4 4, 9 9) \n";
  for(const auto &r : result)
  {
    std::cout << "( " << boost::geometry::get<min_corner, 0>(r) << ", " << boost::geometry::get<min_corner, 1>(r) << " )\n( "
              << boost::geometry::get<max_corner, 0>(r) << ", " << boost::geometry::get<max_corner, 1>(r) << " )\n\n";
  }
  std::list<box> result_slices;
  a = box(point(0, 0), point(5, 5));
  b = box(point(1, 1), point(4, 4));
  boxdiff::box_diff<boxdiff::slice_policy::slices>(a, b, result_slices);
  std::cout << "BOX(0 0, 5 5)\\BOX(1 1, 4 4) (slices)\n";
  for(const auto &r : result_slices)
  {
    std::cout << "valid: " << boost::geometry::is_valid(r) << "\n( " << boost::geometry::get<min_corner, 0>(r) << ", "
              << boost::geometry::get<min_corner, 1>(r) << " )\n( " << boost::geometry::get<max_corner, 0>(r) << ", "
              << boost::geometry::get<max_corner, 1>(r) << " )\n\n";
  }
  std::list<box> result_no_slices;
  boxdiff::box_diff<boxdiff::slice_policy::no_slices>(a, b, result_no_slices);
  std::cout << "BOX(0 0, 5 5)\\BOX(1 1, 4 4) (no_slices)\n";
  for(const auto &r : result_no_slices)
  {
    std::cout << "valid: " << boost::geometry::is_valid(r) << "\n( " << boost::geometry::get<min_corner, 0>(r) << ", "
              << boost::geometry::get<min_corner, 1>(r) << " )\n( " << boost::geometry::get<max_corner, 0>(r) << ", "
              << boost::geometry::get<max_corner, 1>(r) << " )\n\n";
  }
  return 0;
}
