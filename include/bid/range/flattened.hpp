#ifndef BID_RANGE_FLATTENED_HPP_
#define BID_RANGE_FLATTENED_HPP_

#include <bid/range/traits.hpp>

namespace bid
{

namespace flattened_detail
{

template<class Range>
class once_flattened_range
{
private:
    typedef std::decay_t<value_type_t<Range>> lower_dim_range;
    typedef value_type_t<lower_dim_range> value_type;
public:
    typedef range_tag range_tag;

    once_flattened_range() = default;
    once_flattened_range(Range r)
      : higher_dim_(std::move(r))
    {
        while(lower_dim_.empty() && !higher_dim_.empty())
            lower_dim_ = higher_dim_.pop_front();
    }
    
    bool empty() const
    {
        return lower_dim_.empty();
    }

    value_type pop_front_impl()
    {
        assert(!empty());
        value_type result = lower_dim_.pop_front();
        while(lower_dim_.empty() && !higher_dim_.empty())
            lower_dim_ = higher_dim_.pop_front();
        return static_cast<value_type>(result);
    }
private:
    Range higher_dim_;
    lower_dim_range lower_dim_;
};

template<class Range, std::size_t Dimensions>
struct flattened_range_type;

template<class Range>
struct flattened_range_type<Range, 0>
{
    typedef Range type;
};
template<class Range>
struct flattened_range_type<Range, 1>
{
    typedef once_flattened_range<Range> type;
};
template<class Range>
struct flattened_range_type<Range, std::numeric_limits<std::size_t>::max()>
{
    typedef typename flattened_range_type
    <
        Range,
        range_dimension_t<Range>{} - 1
    >::type type;
};
template<class Range, std::size_t Dimensions>
struct flattened_range_type
{
    typedef typename flattened_range_type
    <
        once_flattened_range<Range>,
        Dimensions - 1
    >::type type;
};

}

template<class Range, std::size_t Dimensions = std::numeric_limits<std::size_t>::max()>
using flattened_range = typename flattened_detail::flattened_range_type<Range, Dimensions>::type;

template<std::size_t Dimensions>
struct flattened_transform
{
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    flattened_range<std::decay_t<Range>, Dimensions> operator()(Range&& r) const
    {
        return {{std::forward<Range>(r)}};
    }
};

template<std::size_t Dimensions>
flattened_transform<Dimensions> flattened_by{};

flattened_transform<std::numeric_limits<std::size_t>::max()> flattened;
}

#endif

