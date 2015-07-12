#ifndef BID_RANGE_FRONT_TAKEN_HPP_
#define BID_RANGE_FRONT_TAKEN_HPP_

#include <bid/range/traits.hpp>
#include <bid/range/range_transform.hpp>
#include <bid/range/zipped.hpp>
#include <bid/range/mapped.hpp>

#include <cstddef>

namespace bid
{

struct front_taken_transform
{
    typedef range_transform_tag range_transform_tag;

    std::size_t amount;

    template<class Range>
    decltype(auto) operator()(Range&& r) const
    {
        return zipped(count(0, amount), std::forward<Range>(r)) | projected<1>;
    }
};

front_taken_transform front_taken(std::size_t amount)
{
    return {amount};
}

}

#endif

