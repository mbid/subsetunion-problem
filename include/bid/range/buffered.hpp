#ifndef BID_RANGE_BUFFERED_HPP_
#define BID_RANGE_BUFFERED_HPP_

#include <bid/range/traits.hpp>
#include <bid/range/range_transform.hpp>

#include <boost/optional.hpp>

namespace bid
{

namespace front_detail
{

template<class Range>
class buffered_range_impl
{
private:
    typedef range_tag range_tag;
    typedef value_type_t<Range> value_type;
    typedef size_type_t<Range> size_type;
    static_assert(tmp::list_size<value_type>{} == 1, "ranges with variadic value types can not be buffered");
public:

    buffered_range_impl(Range r)
      : r_(std::move(r))
    {
        get_next();
    }

    bool empty_impl() const
    {
        return !current_value_;
    }
    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& visitor)
    {
        if(current_value_)
        {
            auto _ = scope_guard([&]
            {
                get_next();
            });
            return std::forward<Visitor>(visitor)(std::move(*current_value_));
        }
        
        return std::forward<Visitor>(visitor)(none);
    }
    template<class Visitor>
    decltype(auto) front_impl(Visitor&& visitor) const
    {
        if(current_value_)
            return std::forward<Visitor>(*current_value_);
        else
            return std::forward<Visitor>(none);
    }
private:
    Range r_;
    boost::optional<tmp::list_at<value_type, 0>> current_value_;

    void get_next()
    {
        if(empty(r_))
            current_value_ = boost::none;
        else
        {
            pop_front(r_, [&](auto&& obj)
            {
                current_value_ = std::forward<decltype(obj)>(obj);
            });
        }
    }
};

}

template<class Range>
using buffered_range = std::conditional_t
<
    can_front<Range>{},
    Range,
    front_detail::buffered_range_impl<Range>
>;

struct buffered_transform
{
    template<class Range>
    buffered_range<std::decay_t<Range>> operator()(Range&& r) const
    {
        return {std::forward<Range>(r)};
    }
};

static buffered_transform buffered{};

}

#endif

