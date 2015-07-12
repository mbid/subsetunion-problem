#ifndef BID_RANGE_CONSUME_HPP_ 
#define BID_RANGE_CONSUME_HPP_

#include <bid/tuple.hpp>

#include <bid/range/for_each.hpp>

namespace bid
{

static auto consume = for_each([](auto&&...){});

}

#endif

