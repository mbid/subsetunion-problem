#ifndef BID_DROP_FRONT_INTRINSICALLY_HPP_
#define BID_DROP_FRONT_INTRINSICALLY_HPP_

#include <bid/range/traits/size_type.hpp>

#include <utility>

namespace bid
{

enum class drop_front_method
{
    NONE,
    DROP_FRONT,
    DROP_FRONT_AMOUNT,
    POP_FRONT // this is not implemented here
};

namespace drop_front_intrinsically_detail
{

template<class Range>
struct has_drop_front_impl
{
    typedef char (&no)[1];
    typedef char (&yes)[2];
    template<class>
    using yes_wrapper = yes;

    template<class R>
    static yes_wrapper<decltype(std::declval<R>().drop_front_impl())> test(void*);
    template<class R>
    static no test(...);

    static constexpr bool value = sizeof(test<Range>(0)) == sizeof(yes);
};

template<class Range>
struct has_drop_front_amount_impl
{
    typedef char (&no)[1];
    typedef char (&yes)[2];
    template<class>
    using yes_wrapper = yes;

    template<class R>
    static yes_wrapper<decltype(std::declval<R>().drop_front_impl(0))> test(void*);
    template<class R>
    static no test(...);

    static constexpr bool value = sizeof(test<Range>(0)) == sizeof(yes);
};

}

template<class Range>
using has_drop_front = std::integral_constant<bool, drop_front_intrinsically_detail::has_drop_front_impl<Range>::value>;
template<class Range>
using has_drop_front_amount = std::integral_constant<bool, drop_front_intrinsically_detail::has_drop_front_amount_impl<Range>::value>;

template<class Range>
using has_any_drop_front = std::integral_constant<bool, has_drop_front<Range>{} || has_drop_front_amount<Range>{}>;

template<class Range>
using prefered_drop_front_intrinsically_method = std::conditional_t
<
    has_drop_front<Range>{},
    std::integral_constant<drop_front_method, drop_front_method::DROP_FRONT>,
    std::conditional_t
    <
        has_drop_front_amount<Range>{},
        std::integral_constant<drop_front_method, drop_front_method::DROP_FRONT_AMOUNT>,
        std::integral_constant<drop_front_method, drop_front_method::NONE>
    >
>;
template<class Range>
using prefered_drop_front_amount_intrinsically_method = std::conditional_t
<
    has_drop_front_amount<Range>{},
    std::integral_constant<drop_front_method, drop_front_method::DROP_FRONT_AMOUNT>,
    std::conditional_t
    <
        has_drop_front<Range>{},
        std::integral_constant<drop_front_method, drop_front_method::DROP_FRONT>,
        std::integral_constant<drop_front_method, drop_front_method::NONE>
    >
>;

namespace drop_front_intrinsically_detail
{

template<drop_front_method>
struct drop_front_intrinsically_impl;

template<>
struct drop_front_intrinsically_impl<drop_front_method::DROP_FRONT>
{
    template<class Range>
    static void doit(Range&& r)
    {
        std::forward<Range>(r).drop_front_impl();
    }
    template<class Range>
    static void doit(Range&& r, size_type_t<Range> amount)
    {
        for(size_type_t<Range> i = 0; i != amount; ++i)
            r.drop_front_impl();
    }
};
template<>
struct drop_front_intrinsically_impl<drop_front_method::DROP_FRONT_AMOUNT>
{
    template<class Range>
    static void doit(Range&& r)
    {
        std::forward<Range>(r).drop_front_impl(1);
    }
    template<class Range>
    static void doit(Range&& r, size_type_t<Range> amount)
    {
        std::forward<Range>(r).drop_front_impl(amount);
    }
};

}


template<class Range>
void drop_front_intrinsically(Range&& r)
{
    using namespace drop_front_intrinsically_detail;

    typedef std::decay_t<Range> decayed_range;
    static constexpr drop_front_method method = prefered_drop_front_intrinsically_method<decayed_range>{};
    typedef drop_front_intrinsically_impl<method> impl;
    impl::doit(std::forward<Range>(r));
}
template<class Range>
void drop_front_intrinsically(Range&& r, size_type_t<Range> amount)
{
    using namespace drop_front_intrinsically_detail;

    typedef std::decay_t<Range> decayed_range;
    static constexpr drop_front_method method = prefered_drop_front_amount_intrinsically_method<decayed_range>{};
    typedef drop_front_intrinsically_impl<method> impl;
    impl::doit(std::forward<Range>(r), amount);
}

}

#endif

