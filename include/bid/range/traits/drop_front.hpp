#ifndef BID_DROP_FRONT_HPP_
#define BID_DROP_FRONT_HPP_

#include <bid/range/traits/drop_front_intrinsically.hpp>
#include <bid/range/traits/pop_front.hpp>

#include <bid/functor.hpp>

#include <boost/optional.hpp>

namespace bid
{

using boost::none_t;

template<class Range>
using prefered_drop_front_method = std::conditional_t
<
    has_any_drop_front<Range>{},
    prefered_drop_front_intrinsically_method<Range>,
    std::conditional_t
    <
        has_pop_front<Range>{},
        std::integral_constant<drop_front_method, drop_front_method::POP_FRONT>,
        std::integral_constant<drop_front_method, drop_front_method::NONE>
    >
>;
template<class Range>
using prefered_drop_front_amount_method = std::conditional_t
<
    has_any_drop_front<Range>{},
    prefered_drop_front_amount_intrinsically_method<Range>,
    std::conditional_t
    <
        has_pop_front<Range>{},
        std::integral_constant<drop_front_method, drop_front_method::POP_FRONT>,
        std::integral_constant<drop_front_method, drop_front_method::NONE>
    >
>;

template<class Range>
using can_drop_front = std::integral_constant
<
    bool,
    prefered_drop_front_method<Range>{} != drop_front_method::NONE
>;

namespace drop_front_detail
{

template<drop_front_method>
struct drop_front_impl;

template<>
struct drop_front_impl<drop_front_method::POP_FRONT>
{
    template<class Range>
    static void doit(Range&& r)
    {
        pop_front(std::forward<Range>(r), overloaded(
        [](none_t)
        {
            assert(false); // drop_front: empty range
        },
        [](auto&&)
        {}));
    }
    template<class Range>
    static void doit(Range&& r, size_type_t<Range> amount)
    {
        for(size_type_t<Range> i = 0; i != amount; ++i)
        {
            pop_front(std::forward<Range>(r), overloaded(
            [](none_t)
            {
                assert(false); // drop_front: range too short
            },
            [](auto&&)
            {}));
        }
    }
};
template<drop_front_method>
struct drop_front_impl
{
    template<class Range>
    static void doit(Range&& r)
    {
        drop_front_intrinsically(std::forward<Range>(r));
    }
    template<class Range>
    static void doit(Range&& r, size_type_t<Range> amount)
    {
        drop_front_intrinsically(std::forward<Range>(r), amount);
    }
};

}

template<class Range>
void drop_front(Range&& r)
{
    using namespace drop_front_detail;

    typedef std::decay_t<Range> decayed_range;
    static constexpr drop_front_method method = prefered_drop_front_method<decayed_range>{};
    typedef drop_front_impl<method> impl;
    impl::doit(std::forward<Range>(r));
}
template<class Range>
void drop_front(Range&& r, size_type_t<Range> amount)
{
    using namespace drop_front_detail;

    typedef std::decay_t<Range> decayed_range;
    static constexpr drop_front_method method = prefered_drop_front_method<decayed_range>{};
    typedef drop_front_impl<method> impl;
    impl::doit(std::forward<Range>(r), amount);
}

}

#endif

