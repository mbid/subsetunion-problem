#ifndef BID_SCOPE_GUARD_HPP_
#define BID_SCOPE_GUARD_HPP_

#include <utility>

namespace bid
{

template<class Functor>
struct scope_guard_t
{
    scope_guard_t(Functor f)
      : f_(f)
    {}
    ~scope_guard_t()
    {
        f_();
    }
    Functor f_;
};

template<class Functor>
scope_guard_t<std::decay_t<Functor>> scope_guard(Functor&& f)
{
    return {std::forward<Functor>(f)};
}

}

#endif

