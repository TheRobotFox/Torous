#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <iterator>
#include <ranges>
#include <iostream>
#include <algorithm>

namespace anli {

    namespace impl {
        using std::array;
        using namespace std::ranges;
        template<std::size_t N, typename T>
        struct Vec : public array<T,N>
        {
            // Vector Math
            template<typename O>
            constexpr Vec<N, decltype(T()-O())> operator-(const Vec<N, O> &other) const
            {
                return views::zip_transform(std::minus<>(),
                                    *this, other);
            }
            template<typename O>
            constexpr Vec<N, decltype(T()+O())> operator+(const Vec<N,O> &other) const
            {
                return views::zip_transform(std::plus<>(),
                                    *this, other);
            }
            template<typename O>
            constexpr Vec<N, decltype(T()*O())> operator*(O scalar) const
            {
                return *this | views::transform([scalar](auto e){return e*scalar;});
            }
            template<typename O>
            constexpr Vec<N, decltype(T()/O())> operator/(O scalar) const
            {
                return *this | views::transform([scalar](auto e){return e/scalar;});
            }
            /**
            * Scalarprodukt
            */
            template<typename O>
            constexpr decltype(O()*T()) operator*(const Vec<N, O> &other) const
            {
                return fold_left(views::zip_transform(std::multiplies<>(), *this, other), 0,
                                std::plus<>());
            }
            template<range R>
            constexpr Vec(R &&range)
            {
                copy_n(range.begin(), N, this->begin());
            }
            template<typename... Ip>
            constexpr Vec(Ip... init)
                : array<T,N>{static_cast<T>(init)...}
            {}

        };

        template<std::size_t N, typename T>
        Vec<N,T> operator-(const Vec<N,T> &v)
        {
            return views::transform(v, std::negate<>());
        }
        template<std::size_t N, typename T>
        std::ostream& operator<<(std::ostream &out, const Vec<N,T> &v)
        {
            out << '[';
            copy(v | views::transform([](const T& e){return std::to_string(e);})
                | views::join_with(' '),
                std::ostream_iterator<char>(out));

            out << ']';
            return out;
        }
    }

    template<std::size_t N, typename T=double>
    struct Vec : public impl::Vec<N,T>
    {
        using impl::Vec<N,T>::Vec;
    };
    template<typename T>
    struct Vec<3, T> : impl::Vec<3,T>
    {
        using impl::Vec<3,T>::Vec;
        template<typename O>
        constexpr Vec<3, decltype(T()*O())> cross_product(const Vec<3, O> &other) const
        {
            return Vec<3,decltype(T()*O())>{
            (*this)[1]*other[2]-(*this)[2]*other[1],
            (*this)[2]*other[0]-(*this)[0]*other[2],
            (*this)[0]*other[1]-(*this)[1]*other[0]};
        }
    };

}
