#ifndef BID_FRONT_INTRINSICALLY_HPP_
#define BID_FRONT_INTRINSICALLY_HPP_

#include <utility>

namespace bid
{

namespace front_detail
{

struct arbitrary_visitor
{
    template<class T>
    void operator()(T&&)
    {}
};

template<class Range>
struct has_front_impl
{
    typedef char (&no)[1];
    typedef char (&yes)[2];
    template<class>
    using yes_wrapper = yes;

    template<class R>
    static yes_wrapper<decltype(std::declval<R>().front_impl(arbitrary_visitor{}))> test(void*);
    // this does not work for some reason, maybe compiler bug?
    //static yes_wrapper<decltype(std::declval<R>().pop_front([](auto&&){}))> test(void*);
    template<class R>
    static no test(...);

    static constexpr bool value = sizeof(test<const Range>(0)) == sizeof(yes);
};

}

template<class Range>
using has_front = std::integral_constant<bool, front_detail::has_front_impl<Range>::value>;

}

#endif

