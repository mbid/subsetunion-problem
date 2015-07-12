#ifndef BID_RANGE_SIZE_TYPE_HPP_
#define BID_RANGE_SIZE_TYPE_HPP_

#include <cstddef>

namespace bid
{

// TODO
template<class T>
struct size_type
{
    typedef std::size_t type;
};
template<class T>
using size_type_t = typename size_type<T>::type;

}

#endif
