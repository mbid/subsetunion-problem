#ifndef BID_RANGE_POP_FRONT_HPP_
#define BID_RANGE_POP_FRONT_HPP_

#include "pop_front_intrinsically.hpp"
#include "front_intrinsically.hpp"
#include "drop_front_intrinsically.hpp"

#include <bid/tmp.hpp>
#include <bid/scope_guard.hpp>
#include <bid/functor.hpp>

#include <boost/optional.hpp>

namespace bid
{

using boost::none_t;

enum class pop_front_method
{
    NONE,
    POP_FRONT,
    FRONT_AND_DROP_FRONT
};

template<class Range>
using has_front_and_drop_front = std::integral_constant<bool, has_front<Range>{} && has_any_drop_front<Range>{}>;


template<class Range>
using prefered_pop_front_method = std::conditional_t
<
    has_pop_front<Range>{},
    std::integral_constant<pop_front_method, pop_front_method::POP_FRONT>,
    std::conditional_t
    <
        has_front_and_drop_front<Range>{},
        std::integral_constant<pop_front_method, pop_front_method::FRONT_AND_DROP_FRONT>,
        std::integral_constant<pop_front_method, pop_front_method::NONE>
    >
>;

template<class Range>
using can_pop_front = std::integral_constant<bool, prefered_pop_front_method<Range>{} != pop_front_method::NONE>;

namespace pop_front_detail
{

template<pop_front_method method>
struct pop_front_impl;

template<>
struct pop_front_impl<pop_front_method::POP_FRONT>
{
    template<class Range, class Visitor>
    constexpr static decltype(auto) doit(Range&& r, Visitor&& v)
    {
        return std::forward<Range>(r).pop_front_impl(std::forward<Visitor>(v));
    }
};

template<>
struct pop_front_impl<pop_front_method::FRONT_AND_DROP_FRONT>
{
    template<class Range, class Visitor>
    constexpr static decltype(auto) doit(Range&& r, Visitor&& v)
    {
        bool was_empty = false;
        auto _ = scope_guard([&]
        {
            if(!was_empty)
                drop_front_intrinsically(std::forward<Range>(r));
        });
        
        return r.front_impl(overloaded(
        [&](none_t) -> decltype(auto)
        {
            was_empty = true;
            return std::forward<Visitor>(v)(none_t{});
        },
        [&](auto&& obj) -> decltype(auto)
        {
            return std::forward<Visitor>(v)(std::forward<decltype(obj)>(obj));
        }));
    }
};

template<>
struct pop_front_impl<pop_front_method::NONE>
{
    template<class Range, class Visitor>
    constexpr static decltype(auto) doit(Range&& r, Visitor&& v)
    {
        static_assert(sizeof(Range) == 0, "pop_front not available for this range");
    }
};

}

template<class Range, class Visitor>
constexpr decltype(auto) pop_front(Range&& r, Visitor&& v)
{
    using namespace pop_front_detail;
    
    typedef std::decay_t<Range> decayed_range;
    typedef pop_front_impl<prefered_pop_front_method<decayed_range>{}> impl;
    return impl::doit(std::forward<Range>(r), std::forward<Visitor>(v));
}

}

#endif

