#include "material.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



struct material_t mat_materials[MAT_MAX_MATERIALS];
unsigned int mat_used_materails[MAT_MAX_MATERIALS >> 5] = {0};


struct material_t *mat_AllocMaterial()
{
    int i;
    int j;

    for(i = 0; i < MAT_MAX_MATERIALS >> 5; i++)
    {
        for(j = 0; j < 32; j++)
        {
            if(!(mat_used_materails[i] & (1 << j)))
            {
                mat_used_materails[i] |= 1 << j;
                return  &mat_materials[(i << 5) + j];
            }
        }
    }

    return NULL;
}


void mat_FreeMaterial(struct material_t *material)
{
    int material_index;

    if(material)
    {
        material_index = material - mat_materials;
        mat_FreeMaterialIndex(material_index);
    }
}

void mat_FreeMaterialIndex(int material_index)
{
    int int_index;
    int bit_index;

    if(material_index >= 0 && material_index < MAT_MAX_MATERIALS)
    {
        int_index = material_index >> 5;
        bit_index = material_index % 32;
        mat_used_materails[int_index + bit_index] &= ~(1 << bit_index);
    }
}


struct material_t *mat_CreateMaterial(const char *name, int type, vec3_t base, float roughness)
{
    struct material_t *material = NULL;

    if(type > MAT_MATERIAL_TYPE_FIRST && type < MAT_MATERIAL_TYPE_LAST)
    {
        material = mat_AllocMaterial();

        material->name = strdup(name);
        material->data.type = type;
        material->data.base = base;
    }

    return material;
}



void mat_DestroyMaterial(struct material_t *material)
{
    if(material)
    {
        free(material->name);
        mat_FreeMaterial(material);
    }
}








