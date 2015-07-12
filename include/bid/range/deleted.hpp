#ifndef BID_RANGE_DELETED_HPP_
#define BID_RANGE_DELETED_HPP_

#include <bid/range/traits.hpp>

namespace bid
{

template<class Range>
struct copy_deleted_range
  : Range
{
    copy_deleted_range(const copy_deleted_range&) = delete;
    copy_deleted_range(copy_deleted_range&&) = default;

    copy_deleted_range& operator=(const copy_deleted_range&) = delete;
    copy_deleted_range& operator=(copy_deleted_range&&) = default;

    copy_deleted_range(Range r)
      : Range(std::move(r))
    {}
};

struct copy_deleted_transform
{
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    copy_deleted_range<Range> operator()(Range&& r) const
    {
        return {std::forward<Range>(r)};
    }
};

static copy_deleted_transform copy_deleted{};

}

#endif

