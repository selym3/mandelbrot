#ifndef __MANDELBROT_HPP__
#define __MANDELBROT_HPP__

#include <complex>
#include "util/camera.hpp"
#include "util/vec2.hpp"

#ifdef __MULTIPRECISION__
#include <boost/serialization/nvp.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp> 
// boost::multiprecision::cpp_bin_float_single;
#endif

namespace mb
{

    ///////////
    // TYPES //
    ///////////

    using number = double;
    using complex = std::complex<number>;
    using vec2 = util::vec2<number>;
    using camera = util::camera<vec2>;

    ///////////////
    // CONSTANTS //
    ///////////////

    static std::size_t MAX_ITERS = 20;
    static number BOUNDS = number(2);

    /////////////
    // HELPERS //
    /////////////

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