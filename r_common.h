#ifndef R_COMMON_H
#define R_COMMON_H

#define R_MAX_SPHERES 64
#define R_MAX_TRIANGLES 128
#define R_MAX_VERTICES 512
#define R_MAX_TEMPORAL_FRAMES 4


#define R_SPHERE_UNIFORM_BLOCK_BINDING 0
#define R_MATERIAL_UNIFORM_BLOCK_BINDING 1
#define R_TRIANGLE_UNIFORM_BLOCK_BINDING 2
#define R_VERTICE_UNIFORM_BLOCK_BINDING 3

#include "vector.h"

struct batch_t
{
    unsigned int material_index;
    unsigned int start;
    unsigned int count;
};

struct vertex_t
{
    vec4_t position;
    vec4_t normal;
};

struct triangle_t
{
    int vertices[3];
    int material_index;
};


#endif // R_COMMON_H
