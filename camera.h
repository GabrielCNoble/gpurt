#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"

struct camera_t
{
    struct camera_t *next;
    struct camera_t *prev;

    mat3_t orientation;
    vec3_t position;
};


struct camera_t *camera_CreateCamera(const vec3_t &position, const mat3_t &orientation);


#endif // CAMERA_H
