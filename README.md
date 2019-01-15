This is my attempt at Peter Shirley's Ray Tracer in a Weekend book.

The code is a unity build in Handmade Hero style (no virtual functions, all data in POD types, discriminated unions instead of subclasses), and is not organised perfectly but hey, I don't write C++ for a living so don't judge me.

There is a single `build.sh` script which runs the compiler, a `run.sh` script which will run the exectuable and produce a `test.ppm` image.

There is also a `preview.sh` which will open the OS X preview window on the output image.

I found it useful to run all three together like this:

```
./build.sh && ./run.sh && ./preview.sh
```

# Example Outputs

## Diffuse Materials

![diffuse.png](https://raw.githubusercontent.com/johanventer/raytracer/master/examples/diffuse.png)

## Metal Materials

![metal.png](https://raw.githubusercontent.com/johanventer/raytracer/master/examples/metal.png)

## Dielectric Materials

![glass.png](https://raw.githubusercontent.com/johanventer/raytracer/master/examples/glass.png)

## Random spheres

This is the output of the random spheres example at the end of the book.

![spheres.png](https://raw.githubusercontent.com/johanventer/raytracer/master/examples/spheres.png)
