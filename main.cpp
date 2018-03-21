#include <boost/geometry.hpp>
#include <iostream>
#include <vector>

const int dimension = 2;
using point = boost::geometry::model::point<int, dimension, boost::geometry::cs::cartesian>;
using box = boost::geometry::model::box<point>;
using boost::geometry::max_corner;
using boost::geometry::min_corner;


bool difference_correct(const box& minuend, const box& subtrahend, const std::vector<box>& result) {
	bool valid = true;
	auto total_area = boost::geometry::area(minuend);
	box intersection;
	boost::geometry::intersection(minuend,subtrahend,intersection);
	total_area -= boost::geometry::area(intersection);
	for(int i=0;i<result.size();++i) {
		const auto& r = result[i];
		total_area -= boost::geometry::area(r);
		valid = valid && boost::geometry::within(r,minuend);
		boost::geometry::intersection(r,subtrahend,intersection);
		valid = valid && (boost::geometry::area(intersection)==0);
		for(int j=i+1;j<result.size();++j) {
			const auto& r2 = result[j];
			boost::geometry::intersection(r,r2,intersection);
			valid = valid && (boost::geometry::area(intersection)==0);
		}
	}
	
	return valid && total_area == 0;
}

int main(int, char **)
{
	std::vector<box> result;
	box a(point(0, 0), point(10, 10)), b(point(3, 3), point(7, 7));
	boost::geometry::difference(a,b,result);
	std::cout << "BOX(0 0, 10 10)\\BOX(3 3, 7 7)\n\n";
	for(const auto &r : result)
	{
		std::cout << "BOX( " << boost::geometry::get<min_corner, 0>(r) << " " << boost::geometry::get<min_corner, 1>(r) << " , "
			<< boost::geometry::get<max_corner, 0>(r) << " " << boost::geometry::get<max_corner, 1>(r) << " )\n";
	}
	std::cout << "correct? " << (difference_correct(a,b,result)?"yes":"no") << "\n";
	return 0;
}
