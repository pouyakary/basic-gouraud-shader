
# Gouraud Shader

This is a basic Gouraud Shader based on Henri Gouraud's original paper; ["Continues Shading of Curvad Surfaces"](http://page.mi.fu-berlin.de/block/htw-lehre/wise2012_2013/bel_und_rend/skripte/gouraud1971.pdf). It only does it on one surface in a 2D environment as a hobby project, I'm going to write a full global illuminiation ray tracer and the right implementation will be there.

## Building
You must have LLVM or GCC on your system with support of C++11 at least. Also you must make sure you have OpenGL and GLUT installed on your system. GNU Make is also needed. Then you can simply run:

```
% make run
```