# mandelbrot

mandelbrot viewer in c++ 

## todo:

* better pan/zoom (the pan/zoom uses build in sfml-view, but the panning isn't great I think I should scrap my use of view) (this is also necessary because the mapping is FORCEd to be a float)
* coloring schemes
* julia sets
* testing multiprecision / big integer
* improve resize behavoir (it's fine it just resets zoom level)
* add a way to specify pan/zoom when loading