#ifndef BID_RANGE_TUPLE_RANGE_HPP_
#define BID_RANGE_TUPLE_RANGE_HPP_

#include <bid/range/traits.hpp>
#include <bid/range/for_each.hpp>

#include <bid/branch_table.hpp>
#include <bid/tmp.hpp>

#include <tuple>
#include <utility>

namespace bid
{

namespace tuple_range_detail
{

template<class Tuple, class IndexList>
struct make_value_type;
template<class Tuple, std::size_t... Indices>
struct make_value_type<Tuple, std::index_sequence<Indices...>>
{
    typedef tmp::make_unique<tmp::type_list
    <
        decltype(std::get<Indices>(std::declval<Tuple>()))... 
    >> type;
};

// bijection of (for some n in N, where N are the natural numbers WITH zero)
//  
//     M := {(x,y) in N | x <= y and x <= n and y <= n} ~ {k in N | k < #M}

constexpr std::size_t biject_right(std::size_t x, std::size_t y)
{
    return x + y * (y + 1) / 2;
}
constexpr std::size_t biject_left_get_y(std::size_t k)
{
    for(std::size_t y = 0; ; ++y)
    {
        if(y * (y + 1) / 2 <= k && k < (y + 1) * (y + 2) / 2)
            return y;
    }
    throw 1; // not reachable
}
constexpr std::size_t biject_left_get_x(std::size_t k)
{
    std::size_t y = biject_left_get_y(k);
    return k - y * (y + 1) / 2;
}

template<std::size_t EncodedBeginEnd, class Visitor, class Tuple>
bool do_for_each_drop_afterwards
(
    std::integral_constant<std::size_t, EncodedBeginEnd>,
    std::size_t& begin_ref,
    Visitor&& visitor,
    Tuple&& t
)
{
    static constexpr std::size_t begin = biject_left_get_x(EncodedBeginEnd);
    static constexpr std::size_t end = biject_left_get_y(EncodedBeginEnd);
    if(begin == end)
        return false;

    auto indices_tuple = tmp::list_to_tuple<tmp::make_indices<begin, end>>{};

    control_flow::loop control = control_flow::CONTINUE;
    tuple_for_each(indices_tuple, [&](auto index_obj)
    {
        static constexpr std::size_t index = decltype(index_obj){};
        if(control == control_flow::CONTINUE)
        {
            control = visitor(std::get<index>(t));
            begin_ref = index;
        }
    });
    if(control == control_flow::CONTINUE)
        ++begin_ref;

    return control == control_flow::BREAK;
}

}


template<class Tuple>
class tuple_range
{
public:
    typedef range_tag range_tag;
    typedef typename tuple_range_detail::make_value_type<Tuple&, std::make_index_sequence<std::tuple_size<Tuple>{}>>::type value_type;
    typedef tmp::tuple_to_list_impl<Tuple> tuple;

    constexpr tuple_range(Tuple& t)
      : t_(&t),
        begin_(0),
        end_(std::tuple_size<Tuple>{})
    {}

    constexpr std::size_t size_impl() const
    {
        return end_ - begin_;
    }
private:    
    template<class Visitor>
    struct pop_front_index_visitor
    {
        tuple_range& this_;
        Visitor v;

        template<std::size_t Index>
        constexpr decltype(auto) operator()(std::integral_constant<std::size_t, Index>) const
        {
            ++this_.begin_;
            return v(std::get<Index>(*this_.t_));
        }
        constexpr decltype(auto) operator()(std::integral_constant<std::size_t, std::tuple_size<Tuple>{}>) const
        {
            return v(none_t{});
        }
    };
public:
    template<class Visitor>
    constexpr decltype(auto) pop_front_impl(Visitor&& v)
    {
        typedef functor_result_empty_t<decltype(std::forward<Visitor>(v)), value_type> result_list;
        static_assert(tmp::list_size<result_list>{} == 1, "visitor has ambiguous return type");
        typedef tmp::list_at<result_list, 0> result_type;
        pop_front_index_visitor<decltype(std::forward<Visitor>(v))> index_visitor{*this, std::forward<Visitor>(v)};
        return call_with_typed_index<result_type, std::tuple_size<Tuple>{} + 1>(begin_, index_visitor);
    }

    constexpr void drop_front_impl(std::size_t amount)
    {
        assert(amount <= size_impl());
        begin_ += amount;
    }
    constexpr void drop_back_impl(std::size_t amount)
    {
        assert(amount <= size_impl());
        end_ -= amount;
    }
    template<class Visitor>
    bool for_each_drop_afterwards_impl(Visitor&& visitor)
    {
        using namespace tuple_range_detail;
        std::size_t encoded_begin_end = biject_right(begin_, end_);
        static constexpr std::size_t max_encoded = biject_right(std::tuple_size<Tuple>{}, std::tuple_size<Tuple>{});

        auto typed_index_visitor = [&](auto&&... params)
        {
            return do_for_each_drop_afterwards(std::forward<decltype(params)>(params)...);
        };
        return call_with_typed_index<bool, max_encoded + 1>
        (
            encoded_begin_end,
            typed_index_visitor,
            begin_,
            visitor,
            *t_
        );
    }

private:
    Tuple* t_;
    std::size_t begin_;
    std::size_t end_;
};

}

#endif

