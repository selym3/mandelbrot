#ifndef __MANDELBROT_HPP__
#define __MANDELBROT_HPP__

#include <complex>

namespace mb
{
    using number = double;
    using complex = std::complex<number>;

    static std::size_t MAX_ITERS = 20;
    constexpr number BOUNDS = 2;

    std::size_t mandelbrot(const complex& c)
    {
        complex z = 0;

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