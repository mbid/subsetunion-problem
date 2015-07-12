#ifndef BID_RANGE_TRAITS_EMPTY_HPP_
#define BID_RANGE_TRAITS_EMPTY_HPP_

#include <bid/range/traits/size_intrinsically.hpp>
#include <bid/range/traits/empty_intrinsically.hpp>
#include <bid/range/traits/front.hpp>

#include <bid/functor.hpp>

#include <boost/optional.hpp>

namespace bid
{

using boost::none;
using boost::none_t;

enum class empty_method
{
    NONE,
    EMPTY,
    FRONT,
    SIZE
};

template<empty_method M>
using empty_method_constant = std::integral_constant<empty_method, M>;

template<class Range>
using prefered_empty_method = std::conditional_t
<
    has_empty<Range>{},
    empty_method_constant<empty_method::EMPTY>,
    std::conditional_t
    <
        can_front<Range>{},
        std::integral_constant<empty_method, empty_method::FRONT>,
        std::conditional_t
        <
            has_size<Range>{},
            empty_method_constant<empty_method::SIZE>,
            empty_method_constant<empty_method::NONE>
        >
    >
>;

template<class Range>
using can_empty = std::integral_constant<bool, prefered_empty_method<Range>{} != empty_method::NONE>;

namespace empty_detail
{

template<empty_method>
struct empty_impl;

template<>
struct empty_impl<empty_method::EMPTY>
{
    template<class Range>
    static bool doit(const Range& r)
    {
        return r.empty_impl();
    }
};
template<>
struct empty_impl<empty_method::FRONT>
{
    template<class Range>
    static bool doit(const Range& r)
    {
        return front(r, overloaded(
        [](none_t) -> bool
        {
            return true;
        },
        [](auto&&) -> bool
        {
            return false;
        }));
    }
};
template<>
struct empty_impl<empty_method::SIZE>
{
    template<class Range>
    static bool doit(const Range& r)
    {
        return r.size_impl() == 0;
    }
};
template<>
struct empty_impl<empty_method::NONE>
{
    template<class Range>
    static bool doit(const Range& r)
    {
        static_assert(sizeof(r) == 0, "empty not available for this range");
    }
};

}

template<class Range>
bool empty(const Range& r)
{
    using namespace empty_detail;

    static constexpr empty_method method = prefered_empty_method<Range>{};
    typedef empty_impl<method> impl;
    
    return impl::doit(r);
}

}

#endif

