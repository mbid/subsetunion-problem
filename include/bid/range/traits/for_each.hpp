#ifndef BID_RANGE_FOR_EACH_HPP_
#define BID_RANGE_FOR_EACH_HPP_

namespace bid
{

namespace for_each_detail
{

struct for_each_visitor
{
    template<class T>
    void operator()
};

}

template<class Range>
struct has_for_each_intrinsically_impl
{
    typedef char (&no)[1];
    typedef char (&yes)[2];
    template<class>
    using yes_wrapper = yes;

    template<class R>
    static yes_wrapper<decltype(std::declval<R>().pop_front_impl(arbitrary_visitor{}))> test(void*);
    // this does not work for some reason, maybe compiler bug?
    //static yes_wrapper<decltype(std::declval<R>().pop_front([](auto&&){}))> test(void*);
    template<class R>
    static no test(...);

    static constexpr bool value = sizeof(test<Range>(0)) == sizeof(yes);
};

}

#endif

