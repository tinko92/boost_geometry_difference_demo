#ifndef BOXDIFF_H
#define BOXDIFF_H
#include <algorithm>
#include <array>
#include <boost/geometry.hpp>
#include <cmath>
#include <iterator>

namespace boxdiff
{
  // no_slices (default): only allows boxes that are valid with respect to boost::geometry::is_valid
  // slices: allows boxes with sizes as low as 0 along a dimension
  enum class slice_policy
  {
    slices,
    no_slices
  };
  template <typename box, size_t i = 0> struct is_valid_weak
  {
    static inline bool impl(const box &b)
    {
      return boost::geometry::get<boost::geometry::min_corner, i>(b) <=
             boost::geometry::get<boost::geometry::max_corner, i>(b) &&
             is_valid_weak<box, i + 1>::impl(b);
    }
  };
  template <typename box>
  struct is_valid_weak<box, boost::geometry::traits::dimension<typename boost::geometry::traits::point_type<box>::type>::value>
  {
    static inline bool impl(const box &b) { return true; }
  };

  template <typename box, slice_policy> struct validity_helper
  {
  };

  template <typename box> struct validity_helper<box, slice_policy::slices>
  {
    static inline bool impl(const box &b) { return is_valid_weak<box>::impl(b); }
  };

  template <typename box> struct validity_helper<box, slice_policy::no_slices>
  {
    static inline bool impl(const box &b) { return boost::geometry::is_valid(b); }
  };

  template <typename box, int corner, size_t dim, size_t end, size_t i> struct set_for_each
  {
    static inline void impl(
    std::array<box, 2 * boost::geometry::traits::dimension<typename boost::geometry::traits::point_type<box>::type>::value>
    &arr,
    const typename boost::geometry::traits::coordinate_type<typename boost::geometry::traits::point_type<box>::type>::type &val)
    {
      boost::geometry::set<corner, dim>(arr[i], val);
      set_for_each<box, corner, dim, end, i + 1>::impl(arr, val);
    }
  };
  template <typename box, int corner, size_t dim, size_t end> struct set_for_each<box, corner, dim, end, end>
  {
    static inline void impl(
    std::array<box, 2 * boost::geometry::traits::dimension<typename boost::geometry::traits::point_type<box>::type>::value>
    &arr,
    const typename boost::geometry::traits::coordinate_type<typename boost::geometry::traits::point_type<box>::type>::type &val)
    {
    }
  };
  template <typename box, int coord_dim> struct box_diff_rec
  {
    static inline void
    impl(const box &minuend, const box &subtrahend,
         std::array<box, 2 * boost::geometry::traits::dimension<typename boost::geometry::traits::point_type<box>::type>::value>
         &candidates)
    {
      using point_type = typename boost::geometry::traits::point_type<box>::type;
      using coord_type = typename boost::geometry::traits::coordinate_type<point_type>::type;
      using boost::geometry::max_corner;
      using boost::geometry::min_corner;
      const int dimension = boost::geometry::traits::dimension<point_type>::value;
      const auto minuend_min = boost::geometry::get<min_corner, coord_dim>(minuend);
      const auto minuend_max = boost::geometry::get<max_corner, coord_dim>(minuend);
      const auto subtrahend_min = boost::geometry::get<min_corner, coord_dim>(subtrahend);
      const auto subtrahend_max = boost::geometry::get<max_corner, coord_dim>(subtrahend);
      set_for_each<box, min_corner, coord_dim, 2 * coord_dim, 0>::impl(candidates, minuend_min);
      set_for_each<box, max_corner, coord_dim, 2 * coord_dim, 0>::impl(candidates, minuend_max);
      boost::geometry::set<min_corner, coord_dim>(candidates[2 * coord_dim], minuend_min);
      if(std::is_integral<coord_type>::value)
      {
        boost::geometry::set<max_corner, coord_dim>(candidates[2 * coord_dim], subtrahend_min - 1);
        boost::geometry::set<min_corner, coord_dim>(candidates[2 * coord_dim + 1], subtrahend_max + 1);
      }
      else
      {
        boost::geometry::set<max_corner, coord_dim>(candidates[2 * coord_dim],
                                                    std::nextafter(subtrahend_min, std::numeric_limits<coord_type>::lowest()));
        boost::geometry::set<min_corner, coord_dim>(candidates[2 * coord_dim + 1],
                                                    std::nextafter(subtrahend_max, std::numeric_limits<coord_type>::max()));
      }
      boost::geometry::set<max_corner, coord_dim>(candidates[2 * coord_dim + 1], minuend_max);
      set_for_each<box, min_corner, coord_dim, 2 * dimension, 2 * (coord_dim + 1)>::impl(candidates,
                                                                                         std::max(subtrahend_min, minuend_min));
      set_for_each<box, max_corner, coord_dim, 2 * dimension, 2 * (coord_dim + 1)>::impl(candidates,
                                                                                         std::min(subtrahend_max, minuend_max));
      box_diff_rec<box, coord_dim + 1>::impl(minuend, subtrahend, candidates);
    }
  };

  template <typename box>
  struct box_diff_rec<box, boost::geometry::traits::dimension<typename boost::geometry::traits::point_type<box>::type>::value>
  {
    static inline void
    impl(const box &minuend, const box &subtrahend,
         std::array<box, 2 * boost::geometry::traits::dimension<typename boost::geometry::traits::point_type<box>::type>::value>
         candidates)
    {
    }
  };
  template <slice_policy p = slice_policy::no_slices, typename box, typename container>
  void box_diff(const box &minuend, const box &subtrahend, container& c)
  {
    using point_type = typename boost::geometry::traits::point_type<box>::type;
    const int dimension = boost::geometry::traits::dimension<point_type>::value;
    std::vector<box> result;
    if(boost::geometry::disjoint(minuend, subtrahend))
    {
      result.push_back(minuend);
    }
    else
    {
      std::array<box, 2 * dimension> candidates;
      box_diff_rec<box, 0>::impl(minuend, subtrahend, candidates);
      std::copy_if(candidates.cbegin(), candidates.cend(), std::inserter(c,c.end()),
                   [](const box &a) { return validity_helper<box, p>::impl(a); });
    }
  }
}
#endif  // BOXDIFF_H
