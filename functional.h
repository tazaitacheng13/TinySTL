#pragma

#include <cstddef>

namespace mystl {

    template<class Arg1, class Arg2, class Result>
    struct binary_functoin
    {
        using first_argument_type = Arg1;
        using second_argument_type = Arg2;
        using result_type = Result;
    };

    template<class T>
    struct less : public binary_functoin<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const {return x >= y;}
    };

    template<class T>
    struct greater : public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const {return x < y;}
    };


}