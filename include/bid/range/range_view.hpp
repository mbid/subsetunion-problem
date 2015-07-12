#ifndef BID_RANGE_VIEW_HPP_
#define BID_RANGE_VIEW_HPP_

#include <bid/range/traits.hpp>

namespace bid
{

template<class Range>
class range_view
{
public:
    typedef range_tag range_tag;
    typedef value_type_t<Range> value_type;
    typedef size_type_t<Range> size_type;

    range_view(Range& r)
      : r_(&r)
    {}

    range_view(const range_view&) = delete;
    range_view(range_view&&) = default;

    range_view& operator=(const range_view&) = delete;
    range_view& operator=(range_view&&) = default;

    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& visitor)
    {
        return pop_front(*r_, std::forward<Visitor>(visitor));
    }
    template<class Visitor>
    decltype(auto) front_impl(Visitor&& visitor) const
    {
        return front(*r_, std::forward<Visitor>(visitor));
    }
private:
    Range* r_;
};


template<class Range>
range_view<Range> view(Range& r)
{
    return {r};
}

}

#endif

