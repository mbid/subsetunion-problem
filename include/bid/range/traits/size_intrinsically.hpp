#ifndef BID_RANGE_TRAITS_SIZE_INTRINSICALLY_HPP_
#define BID_RANGE_TRAITS_SIZE_INTRINSICALLY_HPP_

#include <bid/range/traits/size_type.hpp>

#include <utility>

namespace bid
{

namespace size_intrinsically_detail
{

template<class Range>
struct has_size_impl
{
    typedef char (&no)[1];
    typedef char (&yes)[2];
    template<class>
    using yes_wrapper = yes;

    template<class R>
    static yes_wrapper<decltype(std::declval<R>().size_impl())> test(void*);
    template<class R>
    static no test(...);

    static constexpr bool value = sizeof(test<Range>(0)) == sizeof(yes);
};

}

template<class Range>
using has_size = std::integral_constant<bool, size_intrinsically_detail::has_size_impl<Range>::value>;

}

#endif

