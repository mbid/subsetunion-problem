#ifndef SUBSEQUENCES_HPP_
#define SUBSEQUENCES_HPP_

#include <cstdint>
#include <array>
#include <type_traits>


typedef uint8_t subset_t;

bool is_element(std::size_t element, subset_t subset)
{
    assert(element < 8);
    return (subset >> element) & 1;
}

subset_t unite(subset_t a, subset_t b)
{
    return a | b;
}

void print_subset(subset_t subset, std::ostream& os)
{
    os << "[";
    bool is_first_element = true;

    for(std::size_t i = 0; i != 8; ++i)
    {
        if(is_element(i, subset))
        {
            if(is_first_element)
            {
                os << i;
                is_first_element = false;
            }
            else
                os << ", " << i;
        }
    }

    os << "]";

    return;
}


template<class It>
void fill_with_subsequences(It& it, std::size_t begin, std::size_t end, subset_t current_subsequence = 0)
{
    if(begin == end)
    {
        *it = current_subsequence;
        ++it;
    }
    else
    {
        fill_with_subsequences(it, begin + 1, end, current_subsequence & ~(1 << begin));
        fill_with_subsequences(it, begin + 1, end, current_subsequence | (1 << begin));
    }
}


inline constexpr std::size_t constpow(std::size_t base, std::size_t exponent)
{
    if(exponent == 0)
        return 1;
    else
        return base * constpow(base, exponent - 1);
}
template<std::size_t N>
using relevant_subsets_t = std::array<subset_t, constpow(2, N)>;

template<unsigned int N>
relevant_subsets_t<N> relevant_subsets()
{
    relevant_subsets_t<N> result;
    auto it = result.begin();
    fill_with_subsequences(it, 0, N);
    return result;
}

#endif

