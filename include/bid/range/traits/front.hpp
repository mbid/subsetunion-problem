#ifndef BID_RANGE_FRONT_HPP_
#define BID_RANGE_FRONT_HPP_

#include "front_intrinsically.hpp"
#include "pop_front_intrinsically.hpp"

#include <bid/tmp.hpp>
#include <bid/scope_guard.hpp>

namespace bid
{

enum class front_method
{
    NONE,
    FRONT,
    COPY_AND_POP_FRONT,
};

template<class Range>
using has_copy_and_pop_front = std::integral_constant<bool, std::is_copy_constructible<Range>{} && has_pop_front<Range>{}>;

template<class Range>
using prefered_front_method = std::conditional_t
<
    has_front<Range>{},
    std::integral_constant<front_method, front_method::FRONT>,
    std::conditional_t
    <
        has_copy_and_pop_front<Range>{},
        std::integral_constant<front_method, front_method::COPY_AND_POP_FRONT>,
        std::integral_constant<front_method, front_method::NONE>
    >
>;

template<class Range>
using can_front = std::integral_constant<bool, prefered_front_method<Range>{} != front_method::NONE>;

namespace front_detail
{

template<front_method method>
struct front_impl;

template<>
struct front_impl<front_method::FRONT>
{
    template<class Range, class Visitor>
    static decltype(auto) doit(Range&& r, Visitor&& v)
    {
        return std::forward<Range>(r).front_impl(std::forward<Visitor>(v));
    }
};

template<>
struct front_impl<front_method::COPY_AND_POP_FRONT>
{
    template<class Range, class Visitor>
    static decltype(auto) doit(Range&& r, Visitor&& v)
    {
        decltype(auto) rc = copy_if_lvalue(std::forward<Range>(r));
        return std::move(rc).pop_front_impl(std::forward<Visitor>(v));
    }
};

template<>
struct front_impl<front_method::NONE>
{
    template<class Range, class Visitor>
    static void doit(Range&&, Visitor&&)
    {
        static_assert(sizeof(Range) == 0, "front not available for this range");
    }
};

}

template<class Range, class Visitor>
decltype(auto) front(Range&& r, Visitor&& v)
{
    using namespace front_detail;
    
    typedef std::decay_t<Range> decayed_range;
    typedef front_impl<prefered_front_method<decayed_range>{}> impl;
    return impl::doit(std::forward<Range>(r), std::forward<Visitor>(v));
}

}
#endif

