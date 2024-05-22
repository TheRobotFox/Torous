#pragma once
#include "Vec.hpp"
#include <array>
#include <bits/ranges_algo.h>
#include <cstddef>
#include <functional>
#include <ranges>
#include <sstream>

namespace anli {
    using std::array;
    using namespace std::ranges;
    template<std::size_t r, std::size_t c, typename T=double>
    struct Matrix : public array<Vec<r, T>, c>
    {
        // Matrix Math
        template<typename O>
        constexpr Matrix<r,c, decltype(T()-O())> operator-(const Matrix<r,c, O> &other) const
        {
            return views::zip_transform(std::minus<>(),
                                *this, other);
        }
        template<typename O>
        constexpr Matrix<r,c, decltype(T()+O())> operator+(const Matrix<r,c, O> &other) const
        {
            return views::zip_transform(std::plus<>(),
                                *this, other);
        }
        template<typename O>
        constexpr Matrix<r,c, decltype(T()*O())> operator*(T scalar) const
        {
            return *this | views::transform([scalar](auto e){return e*scalar;});
        }
        template<typename O>
        constexpr Matrix<r,c, decltype(T()/O())> operator/(O scalar) const
        {
            return *this | views::transform([scalar](auto e){return e/scalar;});
        }

        template<std::size_t nr, typename O>
        constexpr Matrix<nr,c, decltype(T()+O())> operator*(const Matrix<nr,r, O> &m2) const
        {
            return transform_view(m2, [this](auto v){
                return fold_left(zip_transform_view(std::multiplies<>(), v, *this), Vec<r>{0}, std::plus<>());});
        }

        template<typename... Ip>
        constexpr Matrix(Ip... init)
            : array<Vec<r, T>, c>{static_cast<Vec<r,T>>(init)...}
        {}
    };
    template<std::size_t r, std::size_t c, typename T>
    Matrix<r,c,T> operator-(const Matrix<r,c,T> &m)
    {
        return views::transform(m, std::negate<>());
    }
    template<std::size_t r, std::size_t c, typename T>
    std::ostream& operator<<(std::ostream &out, const Matrix<r,c,T> &v)
    {
        out << '[';
        copy(v | views::transform([](auto e){
                std::ostringstream os;
                os << e;
                return os.str();
                })
            | views::join_with(' '),
            std::ostream_iterator<char>(out));

        out << ']';
        return out;
    }
}

int main() {
    anli::Vec<3> v={2,1,6};
    std::cout << -v << std::endl;
    std::cout << v+v/2 << std::endl;
    std::cout << v*2 << std::endl;
    std::cout << v*v << std::endl;

    anli::Vec<3> a={1,0,0};
    anli::Vec<3> b={0,1,0};

    std::cout << a.cross_product(b) << std::endl;

    using namespace anli;
    anli::Matrix<3,2> m1={Vec<3>{1,2,3},Vec<3>{4,5,6}};
    anli::Matrix<2,2> m2={Vec<3>{1,2},Vec<3>{4,5}};

    std::cout << m1 << std::endl;
    std::cout << m2 << std::endl;
    std::cout << m2*m1 << std::endl;
    return 0;
}
