#ifndef BID_RANGE_RANGE_HPP_
#define BID_RANGE_RANGE_HPP_

#include <bid/range/traits.hpp>
#include <bid/range/iterator_range.hpp>
#include <bid/range/tuple_range.hpp>

namespace bid
{

template<class Iterator>
iterator_range<Iterator> range(Iterator begin, Iterator end)
{
    return iterator_range<Iterator>{begin, end};
}

namespace range_detail
{

template<bool IsRange, bool IsTuple>
struct range_impl;

template<>
struct range_impl<false, true>
{
    template<class Tuple>
    static tuple_range<Tuple> doit(Tuple& t)
    {
        return {t};
    }
};
template<>
struct range_impl<true, false>
{
    template<class Range>
    static decltype(auto) doit(Range&& r)
    {
        return std::forward<Range>(r);
    }
};
template<>
struct range_impl<false, false>
{
    template<class Container>
    static decltype(auto) doit(Container&& c)
    {
        return range(c.begin(), c.end());
    }
};

template<class T>
struct is_tuple;

template<class... Types>
struct is_tuple<std::tuple<Types...>>
  : std::integral_constant<bool, true>
{};
template<class T>
struct is_tuple
  : std::integral_constant<bool, false>
{};

}

template<class Rangeifiable>
decltype(auto) range(Rangeifiable&& r)
{
    using namespace range_detail;
    
    typedef std::decay_t<Rangeifiable> decayed_rangeifiable;
    typedef range_impl
    <
        is_range<decayed_rangeifiable>{},
        is_tuple<decayed_rangeifiable>{}
    > impl;

    return impl::doit(std::forward<Rangeifiable>(r));
}

}

#endif

