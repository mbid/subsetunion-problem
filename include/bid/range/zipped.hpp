#ifndef BID_RANGE_ZIPPED_RANGE_HPP_
#define BID_RANGE_ZIPPED_RANGE_HPP_

#include <bid/tuple.hpp>
#include <bid/range/traits.hpp>
#include <bid/range/count.hpp>
#include <bid/range/iterator_range.hpp>
#include <bid/range/range_transform.hpp>

#include <bid/range/range_sink.hpp>

#include <utility>
#include <tuple>
#include <cassert>

namespace bid
{

namespace zipped_detail
{

template<class TypeList, template<class> class HasFunctor, template<class> class CanFunctor>
using function_active = tmp::meta_and
<
    tmp::list_foldl<std::false_type, tmp::list_map<TypeList, HasFunctor>, tmp::meta_or>, // at least one range has function
    tmp::list_foldl<std::true_type, tmp::list_map<TypeList, CanFunctor>, tmp::meta_and> // all can do function
>;

template<class TypeList>
using pop_front_active = function_active<TypeList, has_pop_front, can_pop_front>;
template<class TypeList>
using front_active = function_active<TypeList, has_front, can_front>;

template<class TypeList>
using drop_front_active = function_active<TypeList, has_drop_front, can_drop_front>;
template<class TypeList>
using drop_front_amount_active = function_active<TypeList, has_drop_front_amount, can_drop_front>;

}


template<class... RangeTypes>
class zipped_range
{
public:
    typedef range_tag range_tag;
    typedef tmp::list_to_tuple_each
    <
        tmp::cartesian_product<value_type_t<RangeTypes>...>
    > value_type;
    typedef std::size_t size_type; // TODO: pick largest of size_type<RangeTypes>...

    zipped_range(std::tuple<RangeTypes...> ranges)
      : ranges_(std::move(ranges))
    {}

    template<class Visitor, class = std::enable_if_t<zipped_detail::pop_front_active<first_of<tmp::type_list<RangeTypes...>, Visitor>>{}>>
    decltype(auto) pop_front_impl(Visitor&& v)
    {
        return pop_front_impl(std::forward<Visitor>(v), std::tuple<>{}, std::integral_constant<std::size_t, 0>{});
    }
private:
    template<class Visitor, class Tuple, std::size_t CurrentIndex>
    decltype(auto) pop_front_impl(Visitor&& v, Tuple&& last_tuple, std::integral_constant<std::size_t, CurrentIndex>)
    {
        return pop_front(std::get<CurrentIndex>(ranges_), overloaded(
        [&](none_t) -> decltype(auto)
        {
            return std::forward<Visitor>(v)(none);
        },
        [&](auto&& obj) -> decltype(auto)
        {
            return pop_front_impl
            (
                std::forward<Visitor>(v),
                std::tuple_cat(std::forward<Tuple>(last_tuple), std::forward_as_tuple(std::forward<decltype(obj)>(obj))),
                std::integral_constant<std::size_t, CurrentIndex + 1>{}
            );
        }));
    }
    template<class Visitor, class Tuple>
    decltype(auto) pop_front_impl(Visitor&& v, Tuple&& last_tuple, std::integral_constant<std::size_t, sizeof...(RangeTypes)>)
    {
        return std::forward<Visitor>(v)(std::forward<Tuple>(last_tuple));
    }

public:
    template<class Visitor, class = std::enable_if_t<zipped_detail::front_active<first_of<tmp::type_list<RangeTypes...>, Visitor>>{}>>
    decltype(auto) front_impl(Visitor&& v) const
    {
        return front_impl(std::forward<Visitor>(v), std::tuple<>{}, std::integral_constant<std::size_t, 0>{});
    }
private:
    template<class Visitor, class Tuple, std::size_t CurrentIndex>
    decltype(auto) front_impl(Visitor&& v, Tuple&& last_tuple, std::integral_constant<std::size_t, CurrentIndex>) const
    {
        return front(std::get<CurrentIndex>(ranges_), overloaded(
        [&](none_t) -> decltype(auto)
        {
            return std::forward<Visitor>(v)(none);
        },
        [&](auto&& obj) -> decltype(auto)
        {
            return front_impl
            (
                std::forward<Visitor>(v),
                std::tuple_cat(std::forward<Tuple>(last_tuple), std::forward_as_tuple(std::forward<decltype(obj)>(obj))),
                std::integral_constant<std::size_t, CurrentIndex + 1>{}
            );
        }));
    }
    template<class Visitor, class Tuple>
    decltype(auto) front_impl(Visitor&& v, Tuple&& last_tuple, std::integral_constant<std::size_t, sizeof...(RangeTypes)>) const
    {
        return std::forward<Visitor>(v)(std::forward<Tuple>(last_tuple));
    }

public:
    template<class A = void, class = std::enable_if_t<zipped_detail::drop_front_active<first_of<tmp::type_list<RangeTypes...>, A>>{}>>
    void drop_front_impl()
    {
        tuple_for_each(ranges_, [&](auto&& r)
        {
            drop_front(r);
        });
    }
    template<class A = void, class = std::enable_if_t<zipped_detail::drop_front_amount_active<first_of<tmp::type_list<RangeTypes...>, A>>{}>>
    void drop_front_impl(size_type amount)
    {
        tuple_for_each(ranges_, [&](auto&& r)
        {
            drop_front(r, amount); // TODO: casts required?
        });
    }

private:
    std::tuple<RangeTypes...> ranges_;
};

template<class... Ranges>
auto zipped(Ranges&&... ranges) -> zipped_range<std::decay_t<decltype(range(std::forward<Ranges>(ranges)))>...>
{
    return {std::forward_as_tuple(range(std::forward<Ranges>(ranges))...)};
}

struct enumerated_transform
{
    typedef range_transform_tag range_transform_tag;
    
    template<class Range>
    auto operator()(Range&& r) const
    {
        return zipped(nats(), std::forward<Range>(r));
    }
};

static enumerated_transform enumerated;

template<class Range>
using enumerated_range = decltype(enumerated(std::declval<Range>()));

}

#endif

