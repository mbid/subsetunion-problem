#ifndef BID_RANGE_IS_RANGE_HPP_
#define BID_RANGE_IS_RANGE_HPP_

#include <utility>

namespace bid
{

struct range_tag
{};

namespace range_traits_detail
{

template<class T>
struct is_range_impl
{
    typedef char no;
    static_assert(sizeof(no) == 1, "");

    template<class S>
    static typename S::range_tag* (&test(void*))[2];
    template<class S>
    static no test(...);

    static constexpr bool value = sizeof(test<T>(0)) != sizeof(no);
};

}

template<class T>
using is_range = std::integral_constant<bool, range_traits_detail::is_range_impl<T>::value>;

}

#endif

