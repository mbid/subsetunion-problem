#ifndef RANGE_TRANSFORM_HPP_
#define RANGE_TRANSFORM_HPP_

#include <bid/range/range.hpp>

#include <utility>

namespace bid
{

/*
 * struct range_transform
 * {
 *     auto operator()(auto&& Range); const
 * };
*/

struct range_transform_tag
{};

namespace range_transform_detail
{

template<class T>
struct is_transform_impl
{
    typedef char no;
    static_assert(sizeof(no) == 1, "");

    template<class S>
    static typename S::range_transform_tag (&test(void*))[2];
    template<class S>
    static no test(...);

    static constexpr bool value = sizeof(test<T>(0)) != sizeof(no);
};

}

template<class T>
using is_range_transform_t = std::integral_constant<bool, range_transform_detail::is_transform_impl<T>::value>;


template<class First, class Second>
struct composed_range_transform
{
    typedef range_transform_tag range_transform_tag;

    First first;
    Second second;

    template<class Range>
    auto operator()(Range&& range) const
    {
        return second(first(std::forward<Range>(range)));
    }
};

template<class Lhs, class Rhs>
auto operator>>(Lhs&& lhs, Rhs&& rhs) -> std::enable_if_t<
        is_range_transform_t<std::decay_t<Lhs>>{} && is_range_transform_t<std::decay_t<Rhs>>{},
        composed_range_transform<std::decay_t<Lhs>, std::decay_t<Rhs>>>
{
    return {std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)};
};

template<class Range, class RangeTransform>
auto operator|(Range&& r, RangeTransform&& transform) -> decltype(std::forward<RangeTransform>(transform)(range(std::forward<Range>(r))))
{
    return std::forward<RangeTransform>(transform)(range(std::forward<Range>(r)));
}

}

#endif

