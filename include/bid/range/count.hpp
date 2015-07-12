#ifndef BID_RANGE_COUNT_HPP_
#define BID_RANGE_COUNT_HPP_

#include <bid/range/traits.hpp>

#include <cstddef>
#include <cassert>

namespace bid
{

class nats_range
{
public:
    typedef range_tag range_tag;
    typedef std::size_t value_type;
    typedef std::size_t size_type;
private:
    std::size_t pos_;
public:
    nats_range(std::size_t begin = 0)
      : pos_(begin)
    {}

    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& v)
    {
        return std::forward<Visitor>(v)(pos_++);
    }

    void drop_front_impl(std::size_t amount = 1)
    {
        pos_ += amount;
    }
};

nats_range nats(std::size_t begin = 0)
{
    return {begin};
}

class counting_range
{
public:
    typedef range_tag range_tag;
    typedef std::size_t value_type;
    typedef std::size_t size_type;
private:
    std::size_t begin_;
    std::size_t end_;
public:
    counting_range()
      : begin_(0),
        end_(0)
    {}
    counting_range(std::size_t begin, std::size_t end)
      : begin_(begin),
        end_(end)
    {}

    bool empty_impl() const
    {
        return begin_ == end_;
    }
    std::size_t size_impl() const
    {
        return end_ - begin_;
    }
    
    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& v)
    {
        if(begin_ == end_)
            return std::forward<Visitor>(v)(none);
        else
            return std::forward<Visitor>(v)(begin_++);
    }
    template<class Visitor>
    decltype(auto) front_impl(Visitor&& v) const
    {
        if(begin_ == end_)
            return std::forward<Visitor>(v)(none);
        else
            return std::forward<Visitor>(v)(begin_);
    }

    void drop_front_impl(std::size_t amount)
    {
        assert(amount <= size_impl());
        begin_ += amount;
    }
};

inline counting_range count(std::size_t begin, std::size_t end)
{
    return counting_range{begin, end};
}

}

#endif

