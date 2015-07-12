#ifndef BID_STRING_LITERAL_HPP_
#define BID_STRING_LITERAL_HPP_

#include <cstddef>

namespace bid
{

template<std::size_t N>
class string_literal
{
public:
    constexpr string_literal(std::array<char, N + 1> arr)
      : data_(arr)
    {}

    constexpr char operator[](std::size_t i) const
    {
        return data_[i];
    }
    constexpr std::size_t size() const
    {
        return N;
    }
    constexpr const char* data() const
    {
        return data_.data();
    }
private:
    std::array<char, N + 1> data_;
};

namespace string_literal_detail
{

template<std::size_t N, std::size_t... indices>
constexpr std::array<char, N> to_array(const char (&arr)[N], std::index_sequence<indices...>)
{
    return {{arr[indices]...}};
}

}

template<std::size_t N>
constexpr string_literal<N - 1> make_string_literal(const char (&arr)[N])
{
    using namespace string_literal_detail;

    typedef std::make_index_sequence<N> indices;
    return {to_array(arr, indices{})};
}

template<std::size_t N, std::size_t M>
constexpr bool operator==(const string_literal<N>& lhs, const string_literal<M>& rhs)
{
    if(N != M)
        return false;
    for(std::size_t i = 0; i != N; ++i)
    {
        if(lhs[i] != rhs[i])
            return false;
    }
    return true;
}

template<std::size_t N, std::size_t M>
constexpr bool operator!=(const string_literal<N>& lhs, const string_literal<M>& rhs)
{
    return !(lhs == rhs);
}

namespace string_literal_detail
{

template<std::size_t N, std::size_t M, std::size_t... LhsIndices, std::size_t... RhsIndices>
constexpr string_literal<N + M> operator_plus_impl(const string_literal<N>& lhs, const string_literal<M>& rhs, std::index_sequence<LhsIndices...>, std::index_sequence<RhsIndices...>)
{
    return {{{lhs[LhsIndices]..., rhs[RhsIndices]...}}};
}

}

template<std::size_t N, std::size_t M>
constexpr string_literal<N + M> operator+(const string_literal<N>& lhs, const string_literal<M>& rhs)
{
    using namespace string_literal_detail;
    return operator_plus_impl(lhs, rhs, std::make_index_sequence<N>{}, std::make_index_sequence<M>{});
}

}

#endif

