#ifndef BID_ARRAY_HPP_
#define BID_ARRAY_HPP_

#include <cstddef>

namespace bid
{

template<class T, std::size_t N>
struct array
{
    constexpr T& operator[](std::size_t i)
    {
        return data_[i];
    }
    constexpr const T& operator[](std::size_t i) const
    {
        return data_[i];
    }

    constexpr T* begin()
    {
        return data_;
    }
    constexpr T* end()
    {
        return data_ + N;
    }
    constexpr const T* begin() const
    {
        return data_;
    }
    constexpr const T* end() const
    {
        return data_ + N;
    }

    constexpr static std::size_t size()
    {
        return N;
    }


    T data_[N];
};

}

#endif

