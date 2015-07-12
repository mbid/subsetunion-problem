#ifndef BID_BRANCH_TABLE_HPP_
#define BID_BRANCH_TABLE_HPP_

#include <array>
#include <utility>

namespace bid
{

namespace branch_table_detail
{

template<class Result, class Functor, class... Arguments>
using branch_pointer = Result (*)(Functor, Arguments...);

template<class IntegerSequence, class Functor, class Result, class... Arguments>
struct branch_table;
template<class Index, Index... Indices, class Functor, class Result, class... Arguments>
struct branch_table<std::integer_sequence<Index, Indices...>, Functor, Result, Arguments...>
{
    template<class IntegralConstant>
    static constexpr Result branch_function(Functor f, Arguments... args)
    {
        return f(IntegralConstant{}, static_cast<Arguments>(args)...);
    }
    
    static constexpr const std::array<branch_pointer<Result, Functor, Arguments...>, sizeof...(Indices)> make()
    {
        return {{(&branch_function<std::integral_constant<Index, Indices>>)...}};
    }
};

template<class Result, std::size_t TableSize, class Index, class Functor, class... Arguments>
constexpr Result call_with_typed_index_impl(std::integral_constant<std::size_t, TableSize>, Index index, Functor&& f, Arguments&&... args)
{
    typedef std::make_integer_sequence<Index, TableSize> indices;
    typedef branch_table<indices, decltype(f), Result, decltype(args)...> table;

    //typedef Result (*branch_pointer)(decltype(std::forward<Functor>(f)), decltype(std::forward<Arguments>(args))...);

    assert(index < TableSize);
    return table::make()[index](std::forward<Functor>(f), std::forward<Arguments>(args)...);

}

template<class Result, class Index, class Functor, class... Arguments>
constexpr Result call_with_typed_index_impl(std::integral_constant<std::size_t, 1>, Index index, Functor&& f, Arguments&&... args)
{
    assert(index < 1);
    return std::forward<Functor>(f)(std::integral_constant<std::size_t, 0>{}, std::forward<Arguments>(args)...);
}
template<class Result, class Index, class Functor, class... Arguments>
constexpr Result call_with_typed_index_impl(std::integral_constant<std::size_t, 2>, Index index, Functor&& f, Arguments&&... args)
{
    assert(index < 2);
    switch(index)
    {
    case 0:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 0>{}, std::forward<Arguments>(args)...);
    case 1:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 1>{}, std::forward<Arguments>(args)...);
    default:
        __builtin_unreachable();
    }
}
template<class Result, class Index, class Functor, class... Arguments>
constexpr Result call_with_typed_index_impl(std::integral_constant<std::size_t, 3>, Index index, Functor&& f, Arguments&&... args)
{
    assert(index < 3);
    switch(index)
    {
    case 0:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 0>{}, std::forward<Arguments>(args)...);
    case 1:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 1>{}, std::forward<Arguments>(args)...);
    case 2:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 2>{}, std::forward<Arguments>(args)...);
    default:
        __builtin_unreachable();
    }
}
template<class Result, class Index, class Functor, class... Arguments>
constexpr Result call_with_typed_index_impl(std::integral_constant<std::size_t, 4>, Index index, Functor&& f, Arguments&&... args)
{
    assert(index < 4);
    switch(index)
    {
    case 0:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 0>{}, std::forward<Arguments>(args)...);
    case 1:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 1>{}, std::forward<Arguments>(args)...);
    case 2:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 2>{}, std::forward<Arguments>(args)...);
    case 3:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 3>{}, std::forward<Arguments>(args)...);
    default:
        __builtin_unreachable();
    }
}
template<class Result, class Index, class Functor, class... Arguments>
constexpr Result call_with_typed_index_impl(std::integral_constant<std::size_t, 5>, Index index, Functor&& f, Arguments&&... args)
{
    assert(index < 5);
    switch(index)
    {
    case 0:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 0>{}, std::forward<Arguments>(args)...);
    case 1:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 1>{}, std::forward<Arguments>(args)...);
    case 2:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 2>{}, std::forward<Arguments>(args)...);
    case 3:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 3>{}, std::forward<Arguments>(args)...);
    case 4:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 4>{}, std::forward<Arguments>(args)...);
    default:
        __builtin_unreachable();
    }
}
template<class Result, class Index, class Functor, class... Arguments>
constexpr Result call_with_typed_index_impl(std::integral_constant<std::size_t, 6>, Index index, Functor&& f, Arguments&&... args)
{
    assert(index < 6);
    switch(index)
    {
    case 0:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 0>{}, std::forward<Arguments>(args)...);
    case 1:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 1>{}, std::forward<Arguments>(args)...);
    case 2:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 2>{}, std::forward<Arguments>(args)...);
    case 3:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 3>{}, std::forward<Arguments>(args)...);
    case 4:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 4>{}, std::forward<Arguments>(args)...);
    case 5:
        return std::forward<Functor>(f)(std::integral_constant<std::size_t, 5>{}, std::forward<Arguments>(args)...);
    default:
        __builtin_unreachable();
    }
}

}

template<class Result, std::size_t TableSize, class Index, class Functor, class... Arguments>
constexpr Result call_with_typed_index(Index index, Functor&& f, Arguments&&... args)
{
    using namespace branch_table_detail;

    return call_with_typed_index_impl<Result>
    (
        std::integral_constant<std::size_t, TableSize>{},
        index,
        std::forward<Functor>(f),
        std::forward<Arguments>(args)...
    );
}

}

#endif

