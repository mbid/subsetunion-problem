#ifndef BID_RANGE_COMPARISON_HPP_
#define BID_RANGE_COMPARISON_HPP_

#include <bid/range/zipped.hpp>
#include <bid/tuple.hpp>
#include <bid/range/range_sink.hpp>
#include <bid/range/range_view.hpp>

#include <utility>
#include <array>

namespace bid
{

namespace comparison_detail
{

static auto default_comparison = [](auto&& lhs, auto&& rhs)
{
    return std::forward<decltype(lhs)>(lhs) == std::forward<decltype(rhs)>(rhs);
};

}

template<class Lhs, class Rhs, class Comparison>
bool equal(Lhs&& lhs, Rhs&& rhs, Comparison&& comparison)
{
    auto&& lhs_range = copy_if_lvalue(range(std::forward<Lhs>(lhs)));
    auto&& rhs_range = copy_if_lvalue(range(std::forward<Rhs>(rhs)));

    bool early_return = zipped(view(lhs_range), view(rhs_range)) >>= 
    [&](auto&& lhs_obj, auto&& rhs_obj)
    {
        if(!comparison(std::forward<decltype(lhs_obj)>(lhs_obj), std::forward<decltype(rhs_obj)>(rhs_obj)))
            return control_flow::BREAK;
        else
            return control_flow::CONTINUE;
    };
    if(early_return)
        return false;
    else 
        return empty(lhs_range) && empty(rhs_range);
}

template<class Lhs, class Rhs>
std::enable_if_t<is_range<std::decay_t<Lhs>>{} || is_range<std::decay_t<Rhs>>{}, bool> operator==(Lhs&& lhs, Rhs&& rhs)
{
    return bid::equal(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs), comparison_detail::default_comparison);
}

template<class Lhs, class Rhs>
std::enable_if_t<is_range<std::decay_t<Lhs>>{} || is_range<std::decay_t<Rhs>>{}, bool> operator!=(Lhs&& lhs, Rhs&& rhs)
{
    return !(std::forward<Lhs>(lhs) == std::forward<Rhs>(rhs));
}


template<class ComparedToRange, class RangesTuple, class Comparison>
std::array<bool, std::tuple_size<RangesTuple>{}> simultaneously_equal(ComparedToRange&& compared_to, RangesTuple ranges_tuple, Comparison&& comparison)
{
    using std::tuple_size;
    using std::forward;

    auto index_tuple = tmp::list_to_tuple<tmp::make_indices<0, tuple_size<RangesTuple>{}>>{};

    std::array<bool, tuple_size<RangesTuple>{}> result;
    for(bool& b : result)
        b = true;
    
    forward<ComparedToRange>(compared_to) >>= [&](auto&& compared_to_obj)
    {
        tuple_for_each(index_tuple, [&](auto index_integral_constant)
        {
            static const std::size_t index = decltype(index_integral_constant){};

            auto&& range = std::get<index>(ranges_tuple);
            pop_front(range, overloaded(
            [&](none_t)
            {
                result[index] = false;
            },
            [&](auto&& other_obj)
            {
                result[index] = result[index] && comparison(compared_to_obj, std::forward<decltype(other_obj)>(other_obj));
            }));
        });
    };

    tuple_for_each(index_tuple, [&](auto index_integral_constant)
    {
        static const std::size_t index = decltype(index_integral_constant){};

        auto&& range = std::get<index>(ranges_tuple);
        result[index] = result[index] && empty(range);
    });

    return result;
}

template<class ComparedToRange, class RangesTuple>
decltype(auto) simultaneously_equal(ComparedToRange&& compared_to, RangesTuple&& ranges_tuple)
{
    return simultaneously_equal(std::forward<ComparedToRange>(compared_to), std::forward<RangesTuple>(ranges_tuple), comparison_detail::default_comparison);
}

}

#endif

