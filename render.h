#ifndef RENDER_H
#define RENDER_H

#include "camera.h"

void r_Init();

void r_Finish();

void r_Trace();

void r_SetActiveCamera(struct camera_t *camera);




#endif // RENDER_H
