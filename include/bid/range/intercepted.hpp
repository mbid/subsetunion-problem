#ifndef BID_RANGE_INTERCEPTED_HPP_
#define BID_RANGE_INTERCEPTED_HPP_

#include <bid/range/traits.hpp>

#include <bid/range/range_transform.hpp>
#include <bid/tuple.hpp>
#include <bid/range/deleted.hpp>

namespace bid
{

template<class Range, class Functor>
class intercepted_pop_front_range
{
public:
    typedef range_tag range_tag;
    typedef value_type_t<Range> value_type;
    typedef size_type_t<Range> size_type;

    intercepted_pop_front_range(Range r, Functor f)
      : r_(std::move(r)),
        f_(std::move(f))
    {}

    template<class Visitor>
    decltype(auto) pop_front_impl(Visitor&& v)
    {
        return pop_front(r_, [&](auto&& obj) -> decltype(auto)
        {
            f_(obj); // TODO: make const
            return std::forward<Visitor>(v)(std::forward<decltype(obj)>(obj));
        });
    }
    template<class Visitor>
    decltype(auto) front_impl(Visitor&& v) const
    {
        return front(r_, std::forward<Visitor>(v));
    }
private:
    Range r_;
    Functor f_;
};

template<class Functor>
struct intercepted_pop_front_transform
{
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    intercepted_pop_front_range<std::decay_t<Range>, Functor> operator()(Range&& r) const
    {
        return {std::forward<Range>(r), f_};
    }

    Functor f_;
};

template<class Functor>
intercepted_pop_front_transform<unpacking_functor<std::decay_t<Functor>>> intercepted_pop_front(Functor&& f)
{
    return {{std::forward<Functor>(f)}};
}

template<class Functor>
struct intercepted_construction_transform
{
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    auto operator()(Range&& r) const // TODO: what about rvalue references?
    {
        f(r);
        return std::forward<Range>(r);
    }

    Functor f;
};

template<class Functor>
intercepted_construction_transform<std::decay_t<Functor>> intercepted_construction(Functor&& f)
{
    return {std::forward<Functor>(f)};
}

template<class Container>
auto saved(Container& c)
{
    return intercepted_pop_front(overloaded(
    [](none_t)
    {},
    [&](auto&& obj)
    {
        c.push_back(std::forward<decltype(obj)>(obj));
    })) >> copy_deleted;
}

}

#endif

