#pragma once
#include "Vec.hpp"
#include <array>
#include <bits/ranges_algo.h>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <ranges>
#include <sstream>
#include <cmath>

namespace anli {
    using std::array;
    using namespace std::ranges;
    template<std::size_t c, std::size_t r, typename T=double>
    struct Matrix : public array<Vec<r, T>, c>
    {
        // Matrix Math
        template<typename O>
        constexpr Matrix<c,r, decltype(T()-O())> operator-(const Matrix<c,r, O> &other) const
        {
            return views::zip_transform(std::minus<>(),
                                *this, other);
        }
        template<typename O>
        constexpr Matrix<c,r, decltype(T()+O())> operator+(const Matrix<c,r, O> &other) const
        {
            return views::zip_transform(std::plus<>(),
                                *this, other);
        }
        template<typename O>
        constexpr Matrix<c,r, decltype(T()*O())> operator*(T scalar) const
        {
            return *this | views::transform([scalar](auto e){return e*scalar;});
        }
        template<typename O>
        constexpr Matrix<c,r, decltype(T()/O())> operator/(O scalar) const
        {
            return *this | views::transform([scalar](auto e){return e/scalar;});
        }

        template<std::size_t nc, typename O>
        constexpr Matrix<nc,r, decltype(T()+O())> operator*(const Matrix<nc,c, O> &m2) const
        {
            return m2 | views::transform([this](auto v){return *this*v;});
        }
        constexpr Vec<r, T>& get(std::size_t i)
        {
            return this->at(i);
        }
        constexpr const Vec<r, T>& get(std::size_t i) const
        {
            return this->at(i);
        }

        // Vector Math
        template<typename O>
        constexpr Vec<r, decltype(T()/O())> operator*(Vec<r, O> v) const
        {
            return fold_left(zip_transform_view(std::multiplies<>(),*this,v), Vec<r, O>{0}, std::plus<>());
        }

        // Constructors
        template<range R>
        constexpr Matrix(R &&range)
        {
            copy_n(range.begin(), c, this->begin());
        }
        constexpr Matrix(std::initializer_list<std::initializer_list<T>> init)
        {
            copy_n(init.begin(), c, this->begin());
        }

        template<typename... Ip>
        constexpr Matrix(Ip... init)
            : array<Vec<r, T>, c>{static_cast<Vec<r,T>>(init)...}
        {}

        constexpr static Matrix<3,3> getRotation(Dimension axies, double angle)
        {
            Matrix m = {
                {1,0,0},
                {0,1,0},
                {0,0,1}
            };
            m.get(axies.next()).get(axies.next())=cos(angle);
            m.get(axies.next()).get(axies.next(2))=sin(angle);
            m.get(axies.next(2)).get(axies.next())=-sin(angle);
            m.get(axies.next(2)).get(axies.next(2))=cos(angle);
            return m;
        }
    };
    template<std::size_t r, std::size_t c, typename T>
    Matrix<c,r,T> operator-(const Matrix<c,r,T> &m)
    {
        return views::transform(m, std::negate<>());
    }
    template<std::size_t r, std::size_t c, typename T>
    std::ostream& operator<<(std::ostream &out, const Matrix<c,r,T> &v)
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

    anli::Vec<2> bv={5,2};

    anli::Matrix<2,2> m1={
    {0,1},
    {1,0}
};

    std::cout << m1 << std::endl;
    std::cout << m1*bv << std::endl;
    std::cout << m1*m1 << std::endl;
    std::cout << Matrix<3, 3>::getRotation(Z, 3.14159265358979/2) << std::endl;
    return 0;
}
