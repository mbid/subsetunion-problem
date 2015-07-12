#ifndef BID_RANGE_TRAITS_DIMENSION_HPP_
#define BID_RANGE_TRAITS_DIMENSION_HPP_

#include <utility>

#include <bid/range/traits/is_range.hpp>
#include <bid/range/traits/value_type.hpp>

namespace bid
{

namespace range_traits_detail
{

template<class Range, bool is_range>
struct range_dimension_impl;

template<class Range>
struct range_dimension_impl<Range, false>
{
    static constexpr std::size_t value = 0;
};
template<class Range>
struct range_dimension_impl<Range, true>
{
    static_assert(is_range<Range>{}, "range_dimension: not a range");

    typedef std::decay_t<value_type_t<Range>> decayed_value_type;
    static constexpr std::size_t value = 1 + range_dimension_impl<decayed_value_type, is_range<decayed_value_type>{}>::value;
};

}

template<class Range>
using dimension = std::integral_constant<std::size_t, range_traits_detail::range_dimension_impl<Range, is_range<Range>{}>::value>;

}

#endif

