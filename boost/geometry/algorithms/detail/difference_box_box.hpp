// Boost.Geometry (aka GGL, Generic Geometry Library)

// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_DIFFERENCE_BOX_BOX_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_DIFFERENCE_BOX_BOX_HPP
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/geometry/util/range.hpp>
#include <boost/geometry/algorithms/is_valid.hpp>

namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace difference
{

template 
<
    typename OutBox,
    int corner,
    size_t dim,
    size_t end,
    size_t i
>
struct set_for_each {
    typedef typename traits::point_type<OutBox>::type PointOut;
    typedef typename traits::coordinate_type<PointOut>::type CoordinatesOut;
        static inline void impl(
            OutBox arr[2 * dim],
            const CoordinatesOut &val)
        {
            set<corner, dim>(arr[i], val);
            set_for_each<OutBox, corner, dim, end, i + 1>::impl(arr, val);
        }
    };

template 
<
    typename OutBox,
    int corner,
    size_t dim,
    size_t end
>
struct set_for_each<OutBox, corner, dim, end, end> {
    typedef typename traits::point_type<OutBox>::type PointOut;
    typedef typename traits::coordinate_type<PointOut>::type CoordinatesOut;
        static inline void impl(
            OutBox arr[2*dim],
            const CoordinatesOut &val) { }
};

template 
<
    typename Box1,
    typename Box2,
    typename OutBox,
    int coord_dim,
    int dim
>
struct box_diff_rec {
    static inline void
        impl(const Box1 &minuend, const Box2 &subtrahend,
            OutBox (&candidates)[2*dim]) {
    typedef typename traits::point_type<Box1>::type Point1;
    typedef typename traits::point_type<Box2>::type Point2;
    typedef typename traits::coordinate_type<Point1>::type Coordinates1;
    typedef typename traits::coordinate_type<Point2>::type Coordinates2;
    const Coordinates1 minuend_min = get<min_corner, coord_dim>(minuend);
    const Coordinates1 minuend_max = get<max_corner, coord_dim>(minuend);
    const Coordinates2 subtrahend_min = get<min_corner, coord_dim>(subtrahend);
    const Coordinates2 subtrahend_max = get<max_corner, coord_dim>(subtrahend);
            
    set_for_each<OutBox, min_corner, coord_dim, 2 * coord_dim, 0>::impl(candidates, minuend_min);
    set_for_each<OutBox, max_corner, coord_dim, 2 * coord_dim, 0>::impl(candidates, minuend_max);
    set<min_corner, coord_dim>(candidates[2 * coord_dim], minuend_min);
    set<max_corner, coord_dim>(candidates[2 * coord_dim], subtrahend_min);
    set<min_corner, coord_dim>(candidates[2 * coord_dim+1], subtrahend_max);
    set<max_corner, coord_dim>(candidates[2 * coord_dim+1], minuend_max);
    set_for_each<OutBox, min_corner, coord_dim, 2 * dim, 2 * (coord_dim + 1)>::impl(candidates,
        std::max(subtrahend_min, minuend_min));
    set_for_each<OutBox, max_corner, coord_dim, 2 * dim, 2 * (coord_dim + 1)>::impl(candidates,
    std::min(subtrahend_max, minuend_max));
        box_diff_rec<Box1, Box2, OutBox, coord_dim + 1,dim>::impl(minuend, subtrahend, candidates);
    }
};

template <
    typename Box1,
    typename Box2,
    typename OutBox,
    int dim
>
struct box_diff_rec<Box1, Box2, OutBox, dim, dim>
    {
        static inline void
            impl(const Box1 &minuend, const Box2 &subtrahend,
                OutBox (&candidates)[2 * dim])
        {
        }
    };

template
<
    typename Box1,
    typename Box2,
    typename BoxCollection
>
struct difference_box_box {
    typedef typename BoxCollection::value_type OutBox;
    typedef typename traits::point_type<Box1>::type Point1;
    typedef typename traits::point_type<Box2>::type Point2;
    typedef typename traits::point_type<OutBox>::type PointOut;
    typedef typename traits::coordinate_type<Point1>::type Coordinates1;
    typedef typename traits::coordinate_type<Point2>::type Coordinates2;
    static const std::size_t dim = dimension<PointOut>::type::value;



    static inline void apply(Box1 const& minuend,
        Box2 const& subtrahend,
        BoxCollection& output_collection)
    {
        range::back_insert_iterator<BoxCollection> out(output_collection);
        if (boost::geometry::disjoint(minuend, subtrahend))
        {
            OutBox m;
            boost::geometry::transform(minuend, m);
            out=m;
        }
        else
        {
            OutBox candidates[2 * dim];
            box_diff_rec<Box1, Box2, OutBox, 0, dim>::impl(minuend, subtrahend, candidates);
            BOOST_FOREACH(OutBox& candidate, candidates)
            {
                if (boost::geometry::is_valid(candidate))
                    *out++ = candidate;
            }
        }
    }
};

}} // namespace detail::difference
#endif // DOXYGEN_NO_DETAIL


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_DIFFERENCE_BOX_BOX_HPP
