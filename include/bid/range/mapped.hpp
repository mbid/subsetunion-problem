#ifndef BID_RANGE_MAPPED_HPP_
#define BID_RANGE_MAPPED_HPP_

#include <utility>
#include <type_traits>

#include <bid/range/traits.hpp>
#include <bid/range/at.hpp>
#include <bid/range/range_transform.hpp>
#include <bid/tuple.hpp>

namespace bid
{

template<class Range, class Functor>
class mapped_range 
{
private:
    Range range_;
    Functor functor_;
public:
    typedef range_tag range_tag;
    typedef functor_result_t<Functor, value_type_t<Range>> value_type;
    typedef size_type_t<Range> size_type;

    mapped_range() = default;
    mapped_range(Range range, Functor functor)
      : range_(std::move(range)),
        functor_(std::move(functor))
    {}

    template<class A = void, class = std::enable_if_t<has_empty<first_of<Range, A>>{}>>
    bool empty_impl() const
    {
        return range_.empty_impl();
    }
    template<class A = void, class = std::enable_if_t<has_size<first_of<Range, A>>{}>>
    size_type_t<Range> size_impl() const
    {
        return range_.size_impl();
    }

    template<class Visitor, class = std::enable_if_t<has_pop_front<first_of<Range, Visitor>>{}>>
    decltype(auto) pop_front_impl(Visitor&& visitor)
    {
        return range_.pop_front_impl(overloaded(
        [&](none_t) -> decltype(auto)
        {
            return std::forward<Visitor>(visitor)(none);
        },
        [&](auto&& obj) -> decltype(auto)
        {
            // TODO: add const
            return std::forward<Visitor>(visitor)(functor_(std::forward<decltype(obj)>(obj)));
        }));
    }
    template<class Visitor, class = std::enable_if_t<has_front<first_of<Range, Visitor>>{}>>
    decltype(auto) front_impl(Visitor&& visitor) const
    {
        return range_.front_impl(overloaded(
        [&](none_t) -> decltype(auto)
        {
            return std::forward<Visitor>(visitor)(none);
        },
        [&](auto&& obj) -> decltype(auto)
        {
            // TODO: add const
            return std::forward<Visitor>(visitor)(functor_(std::forward<decltype(obj)>(obj)));
        }));
    }

    template<class A = void, class = std::enable_if_t<has_drop_front<first_of<Range, A>>{}>>
    void drop_front_impl()
    {
        range_.drop_front_impl();
    }
    template<class A = void, class = std::enable_if_t<has_drop_front_amount<first_of<Range, A>>{}>>
    void drop_front_impl(size_type_t<Range> amount)
    {
        range_.drop_front_impl(amount);
    }
};

template<class Functor>
struct map_transform
{
    Functor f;
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    mapped_range<std::decay_t<Range>, Functor> operator()(Range&& range) const
    {
        return {std::forward<Range>(range), f};
    }
};

template<class Functor>
map_transform<unpacking_functor<std::decay_t<Functor>>> mapped(Functor&& f) 
{
    return {{std::forward<Functor>(f)}};
}

template<class IndexRange>
struct permute_transform
{
    IndexRange indices;
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    decltype(auto) operator()(Range&& permuted_range) const
    {
        return indices | mapped([permuted_range](size_t index)
        {
            return at(permuted_range, index, [&](auto&& obj)
            {
                // TODO: use type_for_storage here
                return std::forward<decltype(obj)>(obj);
            });
        });
    }
};

template<class IndexRange>
auto permuted(IndexRange&& indices) -> permute_transform<std::decay_t<decltype(range(std::forward<IndexRange>(indices)))>>
{
    return {range(std::forward<IndexRange>(indices))};
}

template<std::size_t N>
struct project_functor
{
    template<class Tuple>
    decltype(auto) operator()(Tuple&& t) const
    {
        return std::get<N>(std::forward<Tuple>(t));
    }
};

template<std::size_t N>
struct project_transform
{
    typedef range_transform_tag range_transform_tag;

    template<class Range>
    decltype(auto) operator()(Range&& r) const
    {
        return std::forward<Range>(r) | mapped(project_functor<N>{});
    }
};

template<std::size_t N>
project_transform<N> projected{};

static auto moved = mapped([](auto& obj) -> decltype(auto)
{
    return std::move(obj);
});

}

#endif

