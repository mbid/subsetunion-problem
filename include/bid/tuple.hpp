#ifndef BID_TUPLE_HPP_
#define BID_TUPLE_HPP_

#include <tuple>
#include <utility>
#include <type_traits>

namespace tuple_utils_detail
{

template<class Tuple>
using tuple_indices_t = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>{}>;

template<class F, class Tuple, std::size_t... I>
constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
    return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}

template<class IntegerSequence>
struct integer_sequence_pop_front;
template<class T, T Head, T... Tail>
struct integer_sequence_pop_front<std::integer_sequence<T, Head, Tail...>>
{
    typedef std::integer_sequence<T, Tail...> type;
};

template<class IntegerSequence>
using integer_sequence_pop_front_t = typename integer_sequence_pop_front<IntegerSequence>::type;

}

template<class F, class Tuple>
constexpr decltype(auto) tuple_apply(F&& f, Tuple&& t)
{
    using namespace tuple_utils_detail;
    return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), tuple_indices_t<Tuple>{});
}

namespace tuple_detail
{

template<class Functor, class Argument>
struct can_call_impl
{
    typedef char yes;
    typedef char (&no)[2];
    static_assert(sizeof(yes) != sizeof(no), "");

    template<class>
    struct type_wrapper
    {};
    template<class F>
    static yes test(type_wrapper<std::result_of_t<F(Argument)>>*);
    template<class F>
    static no test(...);

    static constexpr bool value = sizeof(test<Functor>(0)) != sizeof(no);
};

template<class Functor, class Argument>
using can_call = std::integral_constant<bool, can_call_impl<Functor, Argument>::value>;

template<class T>
struct is_tuple
  : std::false_type
{};
template<class... Types>
struct is_tuple<std::tuple<Types...>>
  : std::true_type
{};

}

template<class Functor>
struct unpacking_functor
{
    template
    <
        class Tuple,
        class = std::enable_if_t<tuple_detail::is_tuple<std::decay_t<Tuple>>{}>,
        class = std::enable_if_t<!tuple_detail::can_call<Functor, Tuple&&>{}>
    >
    constexpr decltype(auto) operator()(Tuple&& t)
    {
        return tuple_apply(f, std::forward<Tuple>(t));
    }
    template
    <
        class Tuple,
        class = std::enable_if_t<tuple_detail::is_tuple<std::decay_t<Tuple>>{}>,
        class = std::enable_if_t<!tuple_detail::can_call<const Functor, Tuple&&>{}>
    >
    constexpr decltype(auto) operator()(Tuple&& t) const
    {
        return tuple_apply(f, std::forward<Tuple>(t));
    }

    template
    <
        class T,
        class = std::enable_if_t<tuple_detail::can_call<Functor, T&&>{}>
    >
    constexpr decltype(auto) operator()(T&& obj)
    {
        return f(std::forward<T>(obj));
    }
    template
    <
        class T,
        class = std::enable_if_t<tuple_detail::can_call<const Functor, T&&>{}>
    >
    constexpr decltype(auto) operator()(T&& obj) const
    {
        return f(std::forward<T>(obj));
    }

    Functor f;
};

template<class Functor>
constexpr unpacking_functor<Functor> unpacking(Functor&& f)
{
    return {std::forward<Functor>(f)};
}


namespace tuple_utils_detail
{

template<class Functor, class Tuple, std::size_t... Indices>
using decltype_tuple_t = std::tuple<decltype(std::declval<Functor>()(std::get<Indices>(std::declval<Tuple>())))...>;

template<class Functor, class Tuple, std::size_t... Indices>
constexpr auto map_impl(Tuple&& t, Functor&& f, std::index_sequence<Indices...>) -> decltype_tuple_t<decltype(f), decltype(t), Indices...>
{
    return decltype_tuple_t<decltype(f), decltype(t), Indices...>{f(std::get<Indices>(t))...};
}

}

template<class Functor, class Tuple>
constexpr auto tuple_map(Tuple&& t, Functor&& f)
{
    using namespace tuple_utils_detail;
    return map_impl(t, f, tuple_indices_t<Tuple>{});
}

namespace tuple_utils_detail
{

template<class Functor, class LastResult, class Tuple>
constexpr auto foldl_impl(LastResult&& last_result, Tuple&& t, Functor&& f, std::index_sequence<>)
{
    return std::forward<LastResult>(last_result);
}
template<class Functor, class LastResult, class Tuple, std::size_t HeadIndex, std::size_t... TailIndices>
constexpr decltype(auto) foldl_impl(LastResult&& last_result, Tuple&& t, Functor&& f, std::index_sequence<HeadIndex, TailIndices...>)
{
    return foldl_impl(f(last_result, std::get<HeadIndex>(std::forward<Tuple>(t))), std::forward<Tuple>(t), f, std::index_sequence<TailIndices...>{});
}

}

template<class Initial, class Tuple, class Functor>
constexpr decltype(auto) tuple_foldl(Initial&& initial, Tuple&& t, Functor&& f)
{
    using namespace tuple_utils_detail;

    return foldl_impl(std::forward<Initial>(initial), std::forward<Tuple>(t), f, tuple_indices_t<Tuple>{});
}

template<class Functor, class Tuple>
constexpr void tuple_for_each(Tuple&& t, Functor&& f)
{
    struct dummy_type
    {};
    tuple_foldl(dummy_type{}, t, [&](dummy_type dummy, auto&& obj)
    {
        f(std::forward<decltype(obj)>(obj));
        return dummy_type{};
    });
}


#endif

