#include "render.h"
#include "shader.h"
#include "sphere.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "GL/glew.h"

#include <vector>

extern int window_width;
extern int window_height;


struct shader_t *trace_test_shader;

#define R_RAND_SAMPLES 32

unsigned int rand_samples_texture;



#define R_MAX_SPHERES 64
unsigned int r_spheres_uniform_buffer;
unsigned int r_spheres_count;
struct sphere_t *r_spheres_buffer;

int r_samples = 4;



struct camera_t *r_active_camera = NULL;


void r_Init()
{
    trace_test_shader = shd_LoadShader("trace_test.glsl");

    float *rand_samples;

    int x;
    int y;
    int z;

    rand_samples = (float *)calloc(sizeof(float), window_width * window_height * R_RAND_SAMPLES);


    for(z = 0; z < R_RAND_SAMPLES; z++)
    {
        for(y = 0; y < window_height; y++)
        {
            for(x = 0; x < window_width; x++)
            {
                rand_samples[z * window_width * window_height + y * window_width + x] = (float)rand() / (float)RAND_MAX;
            }
        }
    }

    glGenTextures(1, &rand_samples_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, rand_samples_texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R32F, window_width, window_height, R_RAND_SAMPLES, 0, GL_RED, GL_FLOAT, rand_samples);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glGenBuffers(1, &r_spheres_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, r_spheres_uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct sphere_t) * R_MAX_SPHERES, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    r_spheres_buffer = (struct sphere_t *)calloc(sizeof(struct sphere_t), R_MAX_SPHERES);
}

void r_Finish()
{

}

void r_TestTrace()
{
    glUseProgram(trace_test_shader->shader_program);
    glUniform1f(trace_test_shader->r_width, (float)window_width);
    glUniform1f(trace_test_shader->r_height, (float)window_height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, rand_samples_texture);
    glUniform1i(trace_test_shader->r_rand_samples, 0);
    glUniform1i(trace_test_shader->r_max_rand_samples, R_RAND_SAMPLES);

    glUniform1i(trace_test_shader->r_samples, r_samples);

    /*r_spheres_buffer[0].center_radius = (float [4]){0.0, -0.5, -8.0, 1.0};
    r_spheres_buffer[1].center_radius = (float [4]){0.0, -101.2, 0.0, 100.0};*/
    static float f = 0.0;
    static float r = 0.0;
/*
    static float pitch = 0.0;
    static float yaw = 0.0;*/

    r_spheres_buffer[0].center_radius[0] = 0.0;
    r_spheres_buffer[0].center_radius[1] = -0.5 + sin(f);
    r_spheres_buffer[0].center_radius[2] = -8.0;
    r_spheres_buffer[0].center_radius[3] = 1.0;

    r_spheres_buffer[1].center_radius[0] = 0.0;
    r_spheres_buffer[1].center_radius[1] = -101.3;
    r_spheres_buffer[1].center_radius[2] = 0.0;
    r_spheres_buffer[1].center_radius[3] = 100.0;

   /* mat3_t pitch_matrix;
    mat3_t yaw_matrix;

    pitch_matrix.identity();
    yaw_matrix.identity();*/


    //pitch = -0.02;

    //pitch_matrix.rotate_x(pitch, 1);
    //yaw_matrix.rotate_y(yaw, 1);

    //r_active_camera->position[1] = sin(r);
    //r_active_camera->orientation.identity();
    //r_active_camera->orientation.rotate_y(-0.03, 1);
    //r_active_camera->orientation = yaw_matrix * pitch_matrix;

    f += 0.09;
    //r += 0.02;

    //glBindBuffer(GL_UNIFORM_BUFFER, r_spheres_uniform_buffer);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, r_spheres_uniform_buffer, 0, sizeof(struct sphere_t) * R_MAX_SPHERES);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct sphere_t) * R_MAX_SPHERES, r_spheres_buffer);

    glUniform1i(trace_test_shader->r_spheres_count, 2);

    glUniform3fv(trace_test_shader->r_camera_position, 1, r_active_camera->position.m_floats);
    glUniformMatrix3fv(trace_test_shader->r_camera_orientation, 1, GL_FALSE, (float *)r_active_camera->orientation.m_floats);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRectf(-1.0, -1.0, 1.0, 1.0);
}

void r_SetActiveCamera(struct camera_t *camera)
{
    r_active_camera = camera;
}










