#ifndef BID_RANGE_SINK_HPP_
#define BID_RANGE_SINK_HPP_

#include <bid/range/for_each.hpp>
#include <bid/range/iterator_range.hpp>
#include <bid/range/range.hpp>

#include <utility>

namespace bid
{

/*
 * struct range_sink
 * {
 *     auto operator()(auto&& Range); const // may or may not consume range
 * };
*/

namespace range_sink_detail
{

template<class T>
struct is_sink_impl
{
    typedef char no;
    static_assert(sizeof(no) == 1, "");

    template<class S>
    static typename S::range_sink_tag (&test(void*))[2];
    template<class S>
    static no test(...);

    static constexpr bool value = sizeof(test<T>(0)) != sizeof(no);
};

}

template<class T>
using is_range_sink_t = std::integral_constant<bool, range_sink_detail::is_sink_impl<T>::value>;

namespace range_sink_detail
{

template<class Range, class RangeSink>
constexpr decltype(auto) sink_operator_impl(Range&& r, RangeSink&& sink, std::enable_if_t<is_range_sink_t<std::decay_t<RangeSink>>{}>* = nullptr)
{
    return std::forward<RangeSink>(sink)((std::forward<Range>(r)));
}
template<class Range, class Functor>
constexpr decltype(auto) sink_operator_impl(Range&& r, Functor&& f, std::enable_if_t<!is_range_sink_t<std::decay_t<Functor>>{}>* = nullptr)
{
    return for_each(std::forward<Functor>(f))(std::forward<Range>(r));
}

}

template<class Range, class RangeSink>
constexpr auto operator>>=(Range&& r, RangeSink&& sink) -> decltype(range_sink_detail::sink_operator_impl(std::forward<Range>(r), std::forward<RangeSink>(sink)))
{
    return range_sink_detail::sink_operator_impl(std::forward<Range>(r), std::forward<RangeSink>(sink));
}

}

#endif

