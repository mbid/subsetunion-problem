#ifndef BID_RANGE_PRINTING_HPP_
#define BID_RANGE_PRINTING_HPP_

#include <bid/range/range_sink.hpp>
#include <bid/range/traits.hpp>
#include <ostream>

namespace bid
{

template<class Range>
std::enable_if_t<is_range<std::decay_t<Range>>{}, std::ostream&> operator<<(std::ostream& os, Range&& range)
{
    decltype(auto) rc = copy_if_lvalue(std::forward<Range>(range));
    if(empty(rc))
    {
        os << "[]";
        return os;
    }


    os << "[";
    pop_front(rc, overloaded(
    [&](none_t)
    {
        os << "]";
    },
    [&](auto&& obj)
    {
        os << std::forward<decltype(obj)>(obj);
    }));

    std::move(rc) >>= [&](auto&& obj)
    {
        os << ", " << std::forward<decltype(obj)>(obj);   
    };
    os << "]";
    return os;
}

}

#endif

