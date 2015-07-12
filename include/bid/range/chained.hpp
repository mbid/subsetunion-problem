#ifndef BID_RANGE_CHAINED_HPP_
#define BID_RANGE_CHAINED_HPP_

#include <bid/range/traits.hpp>
#include <bid/tmp.hpp>
#include <bid/range/range.hpp>
#include <bid/range/range_sink.hpp>

namespace bid
{

template<class... Ranges>
class chained_range
{
public:
    typedef range_tag range_tag;
    typedef tmp::make_unique<tmp::list_concat<value_type_t<Ranges>...>> value_type;

    chained_range(Ranges... ranges)
      : range_data_(std::move(ranges)...),
        ranges_(range(range_data_))
    {}
    chained_range(const chained_range& that)
      : range_data_(that.range_data_),
        ranges_(range(range_data_)) // TODO: inefficient
    {}
    chained_range& operator=(const chained_range& that)
    {
        range_data_ = that.range_data_;
        ranges_ = range(range_data_); // TODO: inefficient
        return *this;
    }
    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& visitor)
    {
        typedef functor_result_empty_t<decltype(std::forward<Visitor>(visitor)), value_type> result_list;
        static_assert(tmp::list_size<result_list>{} == 1, "visitor result type ambiguous");

        return pop_front_impl(std::forward<Visitor>(visitor), result_list{});
    }
private:
    template<class Visitor, class ResultType>
    decltype(auto) pop_front_impl(Visitor&& visitor, tmp::type_list<ResultType>)
    {
        boost::optional<ResultType> result;
        ranges_.for_each_drop_afterwards_impl([&](auto&& range)
        {
            control_flow::loop control;
            pop_front(range, overloaded(
            [&](none_t)
            {
                control = control_flow::CONTINUE;
            },
            [&](auto&& obj)
            {
                control = control_flow::BREAK;
                result = std::forward<Visitor>(visitor)(std::forward<decltype(obj)>(obj));
            }));

            return control;
        });
        if(result)
            return static_cast<ResultType>(*result);
        else
            return std::forward<Visitor>(visitor)(none_t{});
    }
    template<class Visitor>
    void pop_front_impl(Visitor&& visitor, tmp::type_list<void>)
    {
        bool early_break = ranges_.for_each_drop_afterwards_impl([&](auto&& range)
        {
            control_flow::loop control;
            pop_front(range, overloaded(
            [&](none_t)
            {
                control = control_flow::CONTINUE;
            },
            [&](auto&& obj)
            {
                control = control_flow::BREAK;
                std::forward<Visitor>(visitor)(std::forward<decltype(obj)>(obj));
            }));

            return control;
        });

        if(!early_break)
            std::forward<Visitor>(visitor)(none_t{});
    }
public:
    template<class Visitor>
    bool for_each_impl(Visitor&& visitor)
    {
        return ranges_.for_each_drop_afterwards_impl([&](auto&& range)
        {
            bool early_break = range >>= visitor;
            if(early_break)
                return control_flow::BREAK;
            else
                return control_flow::CONTINUE;
        });
    }
private:
    std::tuple<Ranges...> range_data_;
    tuple_range<std::tuple<Ranges...>> ranges_;   
};

template<class... Rangeifiables>
auto chained(Rangeifiables&&... rangeifiables)
{
    typedef chained_range<std::decay_t<decltype(range(std::forward<Rangeifiables>(rangeifiables)))>...> result_type;
    return result_type{range(std::forward<Rangeifiables>(rangeifiables))...};
}

}

#endif

