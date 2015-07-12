#ifndef BID_RANGE_AT_HPP_
#define BID_RANGE_AT_HPP_

#include <bid/range/traits.hpp>

namespace bid
{

namespace at_detail
{

template<class ReturnType>
struct no_return_functor
{
    [[noreturn]] ReturnType operator()(none_t) const
    {
        std::terminate();
    }
};

}

template<class Range, class Visitor>
decltype(auto) at(Range&& r, size_type_t<Range> index, Visitor&& visitor)
{
    typedef functor_result_t<decltype(std::forward<Visitor>(visitor)), value_type_t<std::decay_t<Range>>> result_list;
    static_assert(tmp::list_size<result_list>{} == 1, "visitor return type is ambiguous");
    typedef tmp::list_at<result_list, 0> result_type;

    auto opt_copy = copy_if_lvalue(std::forward<Range>(r));
    drop_front(opt_copy, index);
    return front(opt_copy, overloaded(
    at_detail::no_return_functor<result_type>{},
    [&](auto&& obj) -> result_type
    {
        return std::forward<Visitor>(visitor)(std::forward<decltype(obj)>(obj));
    }));
}

}

#endif

