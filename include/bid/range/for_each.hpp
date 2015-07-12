#ifndef BID_RANGE_FOR_EACH_HPP_
#define BID_RANGE_FOR_EACH_HPP_

#include <bid/range/traits.hpp>
#include <bid/tuple.hpp>
#include <bid/functor.hpp>

namespace bid
{

namespace control_flow
{

enum loop
{
    CONTINUE,
    BREAK
};

};

struct range_sink_tag
{};

template<class Functor>
struct for_each_visitor_void_return
{
    control_flow::loop& control;
    const Functor& f;

    constexpr void operator()(none_t) const
    {
        control = control_flow::BREAK;
    }
    template<class T>
    constexpr void operator()(T&& obj) const
    {
        f(std::forward<T>(obj));
        control = control_flow::CONTINUE;
    }
};

template<class Functor>
struct for_each_sink
{
    typedef range_sink_tag range_sink_tag;

    unpacking_functor<Functor> f;

    template<class Range>
    constexpr decltype(auto) operator()(Range&& r) const
    {
        typedef functor_result_t<decltype(f), value_type_t<std::decay_t<decltype(r)>>> result_type_list;
        static_assert(tmp::list_size<result_type_list>{} == 1, "for_each visitor has ambiguous return type");
        typedef tmp::list_at<result_type_list, 0> result_type;
        static_assert(std::is_same<void, result_type>{} || std::is_same<control_flow::loop, result_type>{}, "return type of for_each visitor is neither 'void' nor 'mblib::control_flow::loop");
        typedef std::is_same<void, result_type> returns_void;
        return impl(std::forward<Range>(r), returns_void{});
    }

    template<class Range>
    constexpr void impl(Range&& r, std::true_type) const
    {
        // return type of functor is void
        decltype(auto) opt_copy = copy_if_lvalue(std::forward<Range>(r));
        
        control_flow::loop control = control_flow::CONTINUE; // initialized for constexpr
        do
        {
            pop_front(opt_copy, for_each_visitor_void_return<decltype(f)>{control, f});
        } while(control != control_flow::BREAK);
    }
    template<class Range>
    constexpr bool impl(Range&& r, std::false_type) const
    {
        // return type of functor is control_flow::loop
        decltype(auto) opt_copy = copy_if_lvalue(std::forward<Range>(r));

        bool early_break = true;
        control_flow::loop control = control_flow::CONTINUE; // initialized for constexpr
        do
        {
            control = pop_front(opt_copy, overloaded<control_flow::loop>(
            [&](none_t)
            {
                early_break = false;
                return control_flow::BREAK;
            },
            [&](auto&& obj) -> control_flow::loop
            {
                return f(std::forward<decltype(obj)>(obj));
            }));
        } while(control != control_flow::BREAK);

        return early_break;
    }
};

template<class Functor>
constexpr for_each_sink<std::decay_t<Functor>> for_each(Functor&& f)
{
    return {{std::forward<Functor>(f)}};
}

}

#endif

