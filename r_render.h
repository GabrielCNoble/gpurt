#ifndef R_RENDER_H
#define R_RENDER_H

#include "r_common.h"

#include "camera.h"

void r_Init();

void r_Finish();

void r_Trace();

void r_SetActiveCamera(struct camera_t *camera);




#endif // RENDER_H
