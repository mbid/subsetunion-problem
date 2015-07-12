#ifndef BID_RANGE_FILTERED_HPP_
#define BID_RANGE_FILTERED_HPP_

#include <bid/tuple.hpp>

#include <bid/range/traits.hpp>
#include <bid/range/range_transform.hpp>
#include <bid/range/range_view.hpp>
#include <bid/range/range_sink.hpp>

namespace bid
{

template<class Range, class Predicate>
class filtered_range
{
public:
    typedef range_tag range_tag;
    typedef value_type_t<Range> value_type;
    typedef size_type_t<Range> size_type;

    filtered_range(Range range, Predicate predicate)
      : range_(std::move(range)),
        predicate_(std::move(predicate))
    {}
    
    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& v)
    {
        typedef decltype(std::forward<Visitor>(v)(none)) result_type;
        return pop_front_impl(std::forward<Visitor>(v), std::is_same<result_type, void>{});
    }
private:
    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& v, std::integral_constant<bool, false>)
    {
        // return type of Visitor is non-void
        typedef decltype(std::forward<Visitor>(v)(none)) result_type;
        boost::optional<result_type> result;

        view(range_) >>= for_each([&](auto&& obj) -> decltype(auto)
        {
            if(predicate_(obj)) // TODO: add const
            {
                result = std::forward<Visitor>(v)(std::forward<decltype(obj)>(obj));
                return control_flow::BREAK;
            }
            else
                return control_flow::CONTINUE;
        });
        if(!result)
            return std::forward<Visitor>(v)(none);
        else
            return static_cast<result_type>(std::move(*result));
    }
    template<class Visitor>
    void pop_front_impl(Visitor&& v, std::integral_constant<bool, true>) 
    {
        // return type of Visitor is void
        bool early_break = view(range_) >>= [&](auto&& obj) -> decltype(auto)
        {
            if(predicate_(obj)) // TODO: add const
            {
                std::forward<Visitor>(v)(std::forward<decltype(obj)>(obj));
                return control_flow::BREAK;
            }
            else
                return control_flow::CONTINUE;
        };
        if(!early_break)
            std::forward<Visitor>(v)(none_t{});
    }
    Range range_;
    Predicate predicate_;
};

template<class Predicate>
struct filter_transform
{
    Predicate p;
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    filtered_range<std::decay_t<Range>, Predicate> operator()(Range&& range) const
    {
        return {std::forward<Range>(range), p};
    }
};

template<class Predicate>
filter_transform<unpacking_functor<std::decay_t<Predicate>>> filtered(Predicate&& p)
{
    return {{std::forward<Predicate>(p)}};
}

}

#endif

