#ifndef MODEL_H
#define MODEL_H

#include "r_common.h"



struct model_t
{
    int batch_count;
    struct batch_t *batches;
};



void mdl_Init();

void mdl_Finish();



#endif // MODEL_H
