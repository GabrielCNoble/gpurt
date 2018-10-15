#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector.h"

#define MAT_MAX_MATERIALS 32


enum MATERIAL_TYPES
{
    MAT_MATERIAL_TYPE_FIRST = 0,
    MAT_MATERIAL_TYPE_LAMBERT,
    MAT_MATERIAL_TYPE_DIELECTRIC,
    MAT_MATERIAL_TYPE_METAL,
    MAT_MATERIAL_TYPE_LAST,
};



struct material_data_t
{
    vec4_t base;

    int type;
    float roughness;
    float refrection;
    float align1;
};/* glsl vec4 aligned... */


struct material_t
{
    struct material_data_t data;
    char *name;
};


struct material_t *mat_AllocMaterial();

void mat_FreeMaterial(struct material_t *material);

void mat_FreeMaterialIndex(int material_index);


struct material_t *mat_CreateMaterial(const char *name, int type, vec3_t base, float roughness);

void mat_DestroyMaterial(struct material_t *material);



#endif // MATERIAL_H













