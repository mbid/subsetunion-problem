#ifndef BID_RANGE_TRAITS_SIZE_HPP_
#define BID_RANGE_TRAITS_SIZE_HPP_

#include <bid/range/traits/size_intrinsically.hpp>
#include <bid/range/traits/empty.hpp>
#include <bid/range/traits/drop_front.hpp>

namespace bid
{ 

enum class size_method
{
    NONE,
    SIZE,
    DROP_FRONT_EMPTY
};

template<class Range>
using has_drop_front_empty = std::integral_constant
<
    bool,
    can_drop_front<Range>{} && can_empty<Range>{}
>;

template<class Range>
using prefered_size_method = std::conditional_t
<
    has_size<Range>{},
    std::integral_constant<size_method, size_method::SIZE>,
    std::conditional_t
    <
        has_drop_front_empty<Range>{},
        std::integral_constant<size_method, size_method::DROP_FRONT_EMPTY>,
        std::integral_constant<size_method, size_method::NONE>
    >
>;

namespace size_detail
{

template<size_method>
struct size_impl;

template<>
struct size_impl<size_method::SIZE>
{
    template<class Range>
    static size_type_t<std::decay_t<Range>> doit(Range&& r)
    {
        return std::forward<Range>(r).size_impl();
    }
};

template<>
struct size_impl<size_method::DROP_FRONT_EMPTY>
{
    template<class Range>
    static size_type_t<std::decay_t<Range>> doit(Range&& r)
    {
        auto opt_copy = copy_if_lvalue(std::forward<Range>(r));
        size_type_t<std::decay_t<Range>> result = 0;
        while(!empty(opt_copy))
        {
            drop_front(opt_copy);
            ++result;
        }

        return result;;
    }
};

template<>
struct size_impl<size_method::NONE>
{
    template<class Range>
    static size_type_t<std::decay_t<Range>> doit(Range&& r)
    {
        static_assert(sizeof(r) == 0, "size not available for range");
    }
};

}

template<class Range>
size_type_t<std::decay_t<Range>> size(Range&& r)
{
    using namespace size_detail;

    typedef std::decay_t<Range> decayed_range;
    static constexpr size_method method = prefered_size_method<decayed_range>{};
    typedef size_impl<method> impl;

    return impl::doit(std::forward<Range>(r));
}

}

#endif

