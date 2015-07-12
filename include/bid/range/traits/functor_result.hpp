#ifndef BID_RANGE_TRAITS_FUNCTOR_RESULT_HPP_
#define BID_RANGE_TRAITS_FUNCTOR_RESULT_HPP_

#include <bid/range/traits/value_type.hpp>

#include <bid/tmp.hpp>

#include <boost/optional.hpp>

#include <utility>


namespace bid
{

using boost::none;
using boost::none_t;


template<class Functor, class ValueType>
struct functor_result
{
    static_assert(sizeof(ValueType) == 0, "functor_result with non-type_list ValueType is not allowed");
};

template<class Functor, class... Alternatives>
struct assert_can_call;
template<class Functor>
struct assert_can_call<Functor>
{};
template<class>
struct some_type_wrapper{};
template<class Functor, class Head, class... Alternatives>
struct assert_can_call<Functor, Head, Alternatives...>
  : assert_can_call<Functor, Alternatives...>
{
    static_assert(sizeof(some_type_wrapper<decltype(std::declval<Functor>()(std::declval<Head>()))>) != 0, "");
};

template<class Functor, class... Alternatives>
struct functor_result<Functor, tmp::type_list<Alternatives...>>
  : assert_can_call<Functor, Alternatives...>
{
    typedef tmp::make_unique<tmp::type_list<decltype(std::declval<Functor>()(std::declval<Alternatives>()))...>> type;
};

template<class Functor, class ValueType>
using functor_result_t = typename functor_result<Functor, ValueType>::type;

template<class Functor, class ValueType>
struct functor_result_empty
{
    typedef functor_result_t<Functor, tmp::list_concat<ValueType, tmp::type_list<none_t>>> type;
};
template<class Functor, class ValueType>
using functor_result_empty_t = typename functor_result_empty<Functor, ValueType>::type;

}

#endif

