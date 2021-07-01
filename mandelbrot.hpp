#ifndef __MANDELBROT_HPP__
#define __MANDELBROT_HPP__

#include <complex>
#include<boost/serialization/nvp.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp> 

namespace mb
{
    using number = double;
    // using number = boost::multiprecision::cpp_bin_float_single;
    using complex = std::complex<number>;

    static std::size_t MAX_ITERS = 20;
    static number BOUNDS = number(2);

    std::size_t mandelbrot(const complex& c)
    {
        complex z = number(0);

        std::size_t iters = 0;
        for (; iters < MAX_ITERS; ++iters)
        {
            z = (z * z) + c;

            if (std::abs(z) > BOUNDS)
                break;
        }

        return iters;
    }

}

#endif