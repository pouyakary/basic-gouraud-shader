
# Gouraud Shader

This is a basic Gouraud Shader based on Henri Gouraud's original paper; ["Continues Shading of Curvad Surfaces"](http://page.mi.fu-berlin.de/block/htw-lehre/wise2012_2013/bel_und_rend/skripte/gouraud1971.pdf). It only does it on one surface in a 2D environment as a hobby project, I'm going to write a full global illuminiation ray tracer and the right implementation will be there.

## Gallery

This is the normal shader. Both of the bottom sides are black and therefore the top color gates faded through the edges:

![The original gouraud shader](https://user-images.githubusercontent.com/2157285/37250186-2c7875ac-250c-11e8-9668-bfd67399f13c.png
)



This is if you change the `volume_a` and `volume_b` in the [combine_colors](https://github.com/pmkary/gouraud-shader/blob/master/shader.cpp#L232-L244) function and set pure red, green and blue as light sources:

![](https://user-images.githubusercontent.com/2157285/37142964-ea27c0de-22ce-11e8-93cb-e907c9477af3.png)

## Building
You must have LLVM or GCC on your system with support of C++11 at least. Also you must make sure you have OpenGL and GLUT installed on your system. GNU Make is also needed. Then you can simply run:

```
% make run
```

## Acknowledgments
Henri Gouraud for finding the algorithm! and Dr. Pourbenyamin for helping me fix the algorithm.