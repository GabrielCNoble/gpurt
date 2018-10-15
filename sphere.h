#ifndef SPHERE_H
#define SPHERE_H

#include "vector.h"
#include "material.h"


struct sphere_t
{
    vec4_t center_radius;

    int material_index;
    int align0;
    int align1;
    int align2;
};  /* glsl vec4 aligned... */



#endif // SPHERE_H
