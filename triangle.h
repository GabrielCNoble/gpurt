#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

struct triangle_t
{
    vec4_t verts[3];

    int material_index;
    int align0;
    int align1;
    int align2;
};


#endif // TRIANGLE_H
