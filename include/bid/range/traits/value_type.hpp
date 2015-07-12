#ifndef BID_RANGE_TRAITS_VALUE_TYPE_HPP_
#define BID_RANGE_TRAITS_VALUE_TYPE_HPP_

#include <bid/tmp.hpp>

namespace bid
{

namespace value_type_detail
{

template<class Type>
struct put_in_list
{
    typedef tmp::type_list<Type> type;
};
template<class... Types>
struct put_in_list<tmp::type_list<Types...>>
{
    typedef tmp::type_list<Types...> type;
};

}

template<class Range>
struct value_type
{
    typedef typename value_type_detail::put_in_list<typename Range::value_type>::type type;    

    static_assert(std::is_same<tmp::make_unique<type>, type>{}, "value_type list contains duplicates");
};

template<class Range>
using value_type_t = typename value_type<Range>::type;

}

#endif

