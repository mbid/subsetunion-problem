#ifndef BID_TMP_HPP_
#define BID_TMP_HPP_

#include <type_traits>
#include <tuple>

namespace bid
{

template<class A, class B>
struct first_of_impl
{
    typedef A type;
};
template<class A, class B>
using first_of = typename first_of_impl<A, B>::type;

template<class T>
struct type_for_storage
{
    typedef T type;
};
template<class T>
struct type_for_storage<const T>
{
    typedef T type;
};
template<class T>
struct type_for_storage<T&>
{
    typedef T& type;
};
template<class T>
struct type_for_storage<const T&>
{
    typedef const T& type;
};
template<class T>
struct type_for_storage<T&&>
{
    typedef T type;
};
template<class T>
struct type_for_storage<const T&&>
{
    typedef T type;
};

template<class T>
using type_for_storage_t = typename type_for_storage<T>::type;

namespace tmp_detail
{

template<class T>
struct copied_type
{
    typedef T&& type;
};
template<class T>
struct copied_type<T&>
{
    typedef T type;
};
template<class T>
struct copied_type<const T&>
{
    typedef T type;
};

template<class T>
struct copied_type<T&&>
{
    typedef T&& type;
};
template<class T>
struct copied_type<const T&&>
{
    typedef T type;
};

template<class T>
using copied_type_t = typename copied_type<T>::type;

}

template<class T>
constexpr decltype(auto) copy_if_lvalue(T&& obj)
{
    using namespace tmp_detail;
    return static_cast<copied_type_t<decltype(std::forward<T>(obj))>>(std::forward<T>(obj));
}

namespace tmp
{

template<class... Types>
struct type_list
{};

template<class Type, class TypeList>
struct list_contains;
template<class Type>
struct list_contains<Type, type_list<>>
  : std::integral_constant<bool, false>
{};
template<class Type, class Head, class... Tail>
struct list_contains<Type, type_list<Head, Tail...>>
  : std::integral_constant
    <
        bool,
        std::is_same<Type, Head>{} || list_contains<Type, type_list<Tail...>>{}
    >
{};

template<class... TypeLists>
struct list_concat_impl;
template<class OneList>
struct list_concat_impl<OneList>
{
    typedef OneList type;
};
template<class... ATypes, class... BTypes, class... Tail>
struct list_concat_impl<type_list<ATypes...>, type_list<BTypes...>, Tail...>
{
    typedef typename list_concat_impl
    <
        type_list<ATypes..., BTypes...>,
        Tail...
    >::type type;
};
template<class... TypeLists>
using list_concat = typename list_concat_impl<TypeLists...>::type;

template<class TypeList>
struct make_unique_impl;
template<>
struct make_unique_impl<type_list<>>
{
    typedef type_list<> type;
};
template<class Head, class... Tail>
struct make_unique_impl<type_list<Head, Tail...>>
{
    typedef std::conditional_t
    <
        list_contains<Head, typename make_unique_impl<type_list<Tail...>>::type>{},
        typename make_unique_impl<type_list<Tail...>>::type,
        list_concat<type_list<Head>, typename make_unique_impl<type_list<Tail...>>::type>
    > type;
};

template<class TypeList>
using make_unique = typename make_unique_impl<TypeList>::type;

template<class TypeList>
struct list_size;
template<class... Types>
struct list_size<type_list<Types...>>
  : std::integral_constant<std::size_t, sizeof...(Types)>
{};

template<class TypeList, std::size_t Index>
struct list_at_impl;
template<std::size_t Index>
struct list_at_impl<type_list<>, Index>
{
    static_assert(Index == 2412323423, "list_at index out of bounds");
};
template<class Head, class... Tail>
struct list_at_impl<type_list<Head, Tail...>, 0>
{
    typedef Head type;
};
template<class Head, class... Tail, std::size_t Index>
struct list_at_impl<type_list<Head, Tail...>, Index>
{
    typedef typename list_at_impl<type_list<Tail...>, Index - 1>::type type;
};

template<class TypeList, std::size_t Index>
using list_at = typename list_at_impl<TypeList, Index>::type;


template<class ListOfTypeLists>
struct cartesian_product_impl;

template<>
struct cartesian_product_impl<type_list<>>
{
    typedef type_list<type_list<>> type;
};
template<class... Tail>
struct cartesian_product_impl<type_list<type_list<>, Tail...>>
{
    typedef type_list<> type;
};

namespace tmp_detail
{

template<class TypeList, class ListOfTypeLists>
struct concat_each;
template<class TypeList>
struct concat_each<TypeList, type_list<>>
{
    typedef type_list<> type;
};
template<class TypeList, class Head, class... Tail>
struct concat_each<TypeList, type_list<Head, Tail...>>
{
    typedef list_concat 
    <
        type_list<list_concat<TypeList, Head>>,
        typename concat_each<TypeList, type_list<Tail...>>::type
    > type;
};

}

template<class HeadHead, class... HeadTail, class... Tail>
struct cartesian_product_impl<type_list<type_list<HeadHead, HeadTail...>, Tail...>>
{
    typedef typename cartesian_product_impl<type_list<Tail...>>::type tail_product;
    typedef list_concat 
    <
        typename tmp_detail::concat_each<type_list<HeadHead>, tail_product>::type,
        typename cartesian_product_impl<type_list<type_list<HeadTail...>, Tail...>>::type
    > type;
};

template<class... TypeLists>
using cartesian_product = typename cartesian_product_impl<type_list<TypeLists...>>::type;

template<class TypeList>
struct list_to_tuple_impl;
template<class... Types>
struct list_to_tuple_impl<type_list<Types...>>
{
    typedef std::tuple<Types...> type;
};

template<class TypeList>
using list_to_tuple = typename list_to_tuple_impl<TypeList>::type;

template<class ListOfTypeLists>
struct list_to_tuple_each_impl;
template<>
struct list_to_tuple_each_impl<type_list<>>
{
    typedef type_list<> type;
};
template<class Head, class... Tail>
struct list_to_tuple_each_impl<type_list<Head, Tail...>>
{
    typedef list_concat
    <
        type_list<list_to_tuple<Head>>,
        typename list_to_tuple_each_impl<type_list<Tail...>>::type
    > type;
};

template<class ListOfTypeLists>
using list_to_tuple_each = typename list_to_tuple_each_impl<ListOfTypeLists>::type;



template<class TupleType>
struct tuple_to_list_impl;
template<class... Types>
struct tuple_to_list_impl<std::tuple<Types...>>
{
    typedef type_list<Types...> type;
};
template<class TupleType>
using tuple_to_list = typename tuple_to_list_impl<TupleType>::type;

template<std::size_t begin, std::size_t end>
struct make_indices_impl
{
    typedef list_concat
    <
        type_list<std::integral_constant<std::size_t, begin>>,
        typename make_indices_impl<begin + 1, end>::type
    > type;
};
template<std::size_t begin_end>
struct make_indices_impl<begin_end, begin_end>
{
    typedef type_list<> type;
};

template<std::size_t begin, std::size_t end>
using make_indices = typename make_indices_impl<begin, end>::type;

template<template<class> class MetaFunctor, class TypeList>
struct list_map_impl;
template<template<class> class MetaFunctor>
struct list_map_impl<MetaFunctor, type_list<>>
{
    typedef type_list<> type;
};
template<template<class> class MetaFunctor, class Head, class... Tail>
struct list_map_impl<MetaFunctor, type_list<Head, Tail...>>
{
    typedef list_concat
    <
        type_list<MetaFunctor<Head>>,
        typename list_map_impl<MetaFunctor, type_list<Tail...>>::type
    > type;
};
template<class TypeList, template<class> class MetaFunctor>
using list_map = typename list_map_impl<MetaFunctor, TypeList>::type;

template<class Initial, class TypeList, template<class, class> class MetaFunctor>
struct list_foldl_impl;
template<class Initial, template<class, class> class MetaFunctor>
struct list_foldl_impl<Initial, type_list<>, MetaFunctor>
{
    typedef Initial type;
};
template<class Initial, class Head, class... Tail, template<class, class> class MetaFunctor>
struct list_foldl_impl<Initial, type_list<Head, Tail...>, MetaFunctor>
{
    typedef typename list_foldl_impl
    <
        MetaFunctor<Initial, Head>,
        type_list<Tail...>,
        MetaFunctor
    >::type type;
};
template<class Initial, class TypeList, template<class, class> class MetaFunctor>
using list_foldl = typename list_foldl_impl<Initial, TypeList, MetaFunctor>::type;


template<class Lhs, class Rhs>
using meta_or = std::integral_constant<bool, Lhs{} || Rhs{}>;
template<class Lhs, class Rhs>
using meta_and = std::integral_constant<bool, Lhs{} && Rhs{}>;


template<class... TypeLists>
struct zip_impl;
template<class SingleList>
struct zip_impl<SingleList>
{
    typedef SingleList type;
};
template<class FirstList, class SecondList>
struct zip2_impl;
template<>
struct zip2_impl<type_list<>, type_list<>> // needed to resolve ambiguity
{
    typedef type_list<> type;
};
template<class FirstList>
struct zip2_impl<FirstList, type_list<>>
{
    typedef type_list<> type;
};
template<class SecondList>
struct zip2_impl<type_list<>, SecondList>
{
    typedef type_list<> type;
};
template<class FirstHead, class... FirstTail, class SecondHead, class... SecondTail>
struct zip2_impl<type_list<FirstHead, FirstTail...>, type_list<SecondHead, SecondTail...>>
{
    typedef list_concat
    <
        type_list<type_list<FirstHead, SecondHead>>,
        typename zip2_impl<type_list<FirstTail...>, type_list<SecondTail...>>::type
    > type;
};
template<class FirstList, class SecondList> // TODO: make this work for n > 2
struct zip_impl<FirstList, SecondList>
{
    typedef typename zip2_impl<FirstList, SecondList>::type type;
};

template<class... TypeLists>
using zip = typename zip_impl<TypeLists...>::type;

}

}

#endif

