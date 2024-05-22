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

        template<std::size_t N, typename T>
        struct Vec;
        template<std::size_t N, typename T>
        struct VecMemberRef : public VecMemberRef<N-1, T> {
                template<std::size_t S>
                constexpr VecMemberRef(Vec<S, T> &v)
                    : VecMemberRef<N-1, T>(v)
                {}
        };

        template<typename T>
        struct VecMemberRef<0, T> {
            T const& x;
            template<std::size_t S>
            constexpr VecMemberRef(Vec<S, T> &v)
                : x(v.at(0))
            {}
        };
        #define AddMemberRef(I, Name) \
        template<typename T>          \
        struct VecMemberRef<I, T> {   \
            T const& Name;            \
            template<std::size_t S>   \
            constexpr VecMemberRef(Vec<S, T> &v)\
                : Name(v.at(0))       \
            {}                        \
        };
        AddMemberRef(1, y)
        AddMemberRef(2, z)




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
            constexpr double length() const
            {
                return sqrt(fold_left(*this | views::transform([](auto e){return e*e;}),
                                 0,
                                 std::plus<>()));
            }
            constexpr Vec norm() const {
                return *this/length();
            }

            constexpr T& get(std::size_t i)
            {
                return this->at(i);
            }
            constexpr const T& get(std::size_t i) const
            {
                return this->at(i);
            }

            // COnstructors
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
            template<typename... Ip>
            constexpr Vec(Ip... init)
                : impl::Vec<N, T>{init...}
            {}
    };
    template<typename T>
    struct Vec<3, T> : impl::Vec<3,T>
    {
        template<typename... Ip>
        constexpr Vec(Ip... init)
            : impl::Vec<3, T>{init...}
        {}

        template<typename O>
        constexpr Vec<3, decltype(T()*O())> cross_product(const Vec<3, O> &other) const
        {
            return {
                get<1>(*this)*get<2>(other)-get<2>(*this)*get<1>(other),
                get<2>(*this)*get<0>(other)-get<0>(*this)*get<2>(other),
                get<0>(*this)*get<1>(other)-get<1>(*this)*get<0>(other)
            };
        }
    };

    enum DIMENSION : std::size_t {X=0,Y,Z,W};
    struct Dimension {
        std::size_t axies;
        operator size_t()
        {
            return axies;
        }
        Dimension next(std::size_t n=1)
        {
            return (axies+n)%(axies+1);
        }
        Dimension(std::size_t d)
            : axies(d)
        {}
        Dimension(DIMENSION d)
            : axies(d)
        {}

    };
}
