#ifndef BID_RANGE_TAKEN_WHILE_HPP_
#define BID_RANGE_TAKEN_WHILE_HPP_

#include <bid/range/traits.hpp>
#include <bid/range/range_transform.hpp>

#include <utility>

namespace bid
{

template<class Range, class Predicate>
class taken_while_range
{
public:
    typedef range_tag range_tag;
    typedef value_type_t<Range> value_type;
    typedef size_type_t<Range> size_type;

    taken_while_range(Range r, Predicate p)
      : r_(std::move(r)),
        p_(std::move(p))
    {}

    template<class Visitor, class = std::enable_if_t<has_front<first_of<Range, Visitor>>{}>>
    decltype(auto) front_impl(Visitor&& v) const
    {
        return front(r_, overloaded(
        [&](none_t) -> decltype(auto)
        {
            return std::forward<Visitor>(v)(none);
        },
        [&](auto&& obj) -> decltype(auto)
        {
            // TODO: make const
            if(p_(obj))
                return std::forward<Visitor>(v)(std::forward<decltype(obj)>(obj));
            else
                return std::forward<Visitor>(v)(none);
        }));
    }
    template<class Visitor, class = std::enable_if_t<has_pop_front<first_of<Range, Visitor>>{}>>
    decltype(auto) pop_front_impl(Visitor&& v)
    {
        return pop_front(r_, overloaded(
        [&](none_t) -> decltype(auto)
        {
            return std::forward<Visitor>(v)(none);
        },
        [&](auto&& obj) -> decltype(auto)
        {
            // TODO: make const
            if(p_(obj))
                return std::forward<Visitor>(v)(std::forward<decltype(obj)>(obj));
            else
                return std::forward<Visitor>(v)(none);
        }));
    }
    template<class A = void, class = std::enable_if_t<can_drop_front<first_of<Range, A>>{}>>
    void drop_front_impl()
    {
        drop_front(r_);
    }
private:
    Range r_;
    Predicate p_;
};

template<class Predicate>
struct taken_while_transform
{
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    taken_while_range<std::decay_t<Range>, Predicate> operator()(Range&& r) const
    {
        return {std::forward<Range>(r), p};
    }

    Predicate p;
};

template<class Predicate>
taken_while_transform<unpacking_functor<std::decay_t<Predicate>>> taken_while(Predicate&& p)
{
    return {{std::forward<Predicate>(p)}};
}

}

#endif

