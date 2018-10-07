#include "camera.h"

#include <stdlib.h>
#include <stdio.h>

struct camera_t *cam_cameras = NULL;
struct camera_t *cam_last_camera = NULL;


struct camera_t *camera_CreateCamera(const vec3_t &position, const mat3_t &orientation)
{
    struct camera_t *camera;

    camera = (struct camera_t *)calloc(sizeof(struct camera_t ), 1);

    camera->position = position;
    camera->orientation = orientation;

    if(!cam_cameras)
    {
        cam_cameras = camera;
    }
    else
    {
        cam_last_camera->next = camera;
        camera->prev = cam_last_camera;
    }

    cam_last_camera = camera;

    return camera;
}
