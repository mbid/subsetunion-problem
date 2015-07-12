#ifndef BID_RANGE_ITERATOR_RANGE_HPP_
#define BID_RANGE_ITERATOR_RANGE_HPP_

#include <bid/range/traits.hpp>
#include <cstddef>

namespace bid
{

template<class Iterator>
class iterator_range
{
private:
    Iterator begin_;
    Iterator end_;

    template<class T>
    using is_this_type = std::is_same<std::decay_t<T>, iterator_range<Iterator>>;

public:
    typedef range_tag range_tag;
    typedef decltype(*begin_) value_type;

    template<class Container>
    iterator_range(Container&& c, typename std::enable_if<!is_this_type<Container>{}, void*>::type = nullptr)
      : begin_(std::begin(c)),
        end_(std::end(c))
    {}
    iterator_range(Iterator begin, Iterator end)
      : begin_(begin),
        end_(end)
    {}
    
    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& v)
    {
        if(begin_ == end_)
            return std::forward<Visitor>(v)(none);
        else
            return std::forward<Visitor>(v)(*(begin_++));
    }
    template<class Visitor>
    decltype(auto) front_impl(Visitor&& v) const
    {
        if(begin_ == end_)
            return std::forward<Visitor>(v)(none);
        else
            return std::forward<Visitor>(v)(*begin_);
    }
};

}

#endif

