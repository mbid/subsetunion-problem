#ifndef BID_RANGE_TRAITS_EMPTY_INTRINSICALLY_HPP_
#define BID_RANGE_TRAITS_EMPTY_INTRINSICALLY_HPP_

#include <utility>

namespace bid
{

namespace empty_intrinsically_detail
{

template<class Range>
struct has_empty_impl
{
    typedef char (&no)[1];
    typedef char (&yes)[2];
    template<class>
    using yes_wrapper = yes;

    template<class R>
    static yes_wrapper<decltype(std::declval<R>().empty_impl())> test(void*);
    template<class R>
    static no test(...);

    static constexpr bool value = sizeof(test<Range>(0)) == sizeof(yes);
};

}

template<class Range>
using has_empty = std::integral_constant<bool, empty_intrinsically_detail::has_empty_impl<Range>::value>;

}

#endif

