#ifndef __MB_HPP__
#define __MB_HPP__

#include <complex>

namespace window
{
    constexpr std::size_t WIDTH = 600;
    constexpr std::size_t HEIGHT = 600;

    constexpr double DELAY_MS = 1000.0 / 60.0;
}

namespace mb
{
    using number = double;
    using complex = std::complex<number>;

    constexpr number PLANE_WIDTH = 1;
    constexpr number PLANE_HEIGHT = 1;

    constexpr std::size_t MAX_ITERS = 100;
    constexpr number BOUNDS = 2;
}

#endif