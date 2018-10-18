#include "r_render.h"
#include "shader.h"
#include "sphere.h"
//#include "triangle.h"
#include "material.h"
#include "vector.h"

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

#define R_RAND_TEXTURE_WIDTH 64
#define R_RAND_TEXTURE_HEIGHT 64
unsigned int r_rand_texture;




unsigned int r_spheres_uniform_buffer;
unsigned int r_spheres_count;
struct sphere_t *r_spheres_buffer;


unsigned int r_material_uniform_buffer;
struct material_data_t *r_material_buffer;


unsigned int r_triangle_uniform_buffer;
struct triangle_t *r_triangle_buffer;

unsigned int r_vertex_uniform_buffer;
struct vertex_t *r_vertex_buffer;


int r_samples = 1;

struct framebuffer_t
{
    unsigned int framebuffer_id;
    unsigned int colorbuffer_attachment;
};



int r_cur_framebuffer = 0;
struct framebuffer_t r_framebuffers[R_MAX_TEMPORAL_FRAMES];


struct camera_t *r_active_camera = NULL;


void r_Init()
{
    trace_test_shader = shd_LoadShader("trace.glsl");

    float *float_rand_samples;
    float *short_rand_samples;

    //float *rand_samples;

    int x;
    int y;
    int z;


    float_rand_samples = (float *)calloc(sizeof(float), R_RAND_TEXTURE_WIDTH * R_RAND_TEXTURE_HEIGHT * R_RAND_SAMPLES);


    for(z = 0; z < R_RAND_SAMPLES; z++)
    {
        for(y = 0; y < R_RAND_TEXTURE_HEIGHT; y++)
        {
            for(x = 0; x < R_RAND_TEXTURE_WIDTH; x++)
            {
                float_rand_samples[z * R_RAND_TEXTURE_WIDTH * R_RAND_TEXTURE_HEIGHT + y * R_RAND_TEXTURE_WIDTH + x] = (float)rand() / (float)RAND_MAX;
            }
        }
    }

    glGenTextures(1, &r_rand_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, r_rand_texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R32F, R_RAND_TEXTURE_WIDTH, R_RAND_TEXTURE_HEIGHT, R_RAND_SAMPLES, 0, GL_RED, GL_FLOAT, float_rand_samples);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);



    glGenBuffers(1, &r_spheres_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, r_spheres_uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct sphere_t) * R_MAX_SPHERES, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &r_material_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, r_material_uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct material_data_t) * MAT_MAX_MATERIALS, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &r_triangle_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, r_triangle_uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct triangle_t) * R_MAX_TRIANGLES, NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &r_vertex_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, r_vertex_uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct vertex_t) * R_MAX_VERTICES, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    r_spheres_buffer = (struct sphere_t *)calloc(sizeof(struct sphere_t), R_MAX_SPHERES);
    r_material_buffer = (struct material_data_t *)calloc(sizeof(struct material_data_t), MAT_MAX_MATERIALS);
    r_triangle_buffer = (struct triangle_t *)calloc(sizeof(struct triangle_t), R_MAX_TRIANGLES);
    r_vertex_buffer = (struct vertex_t *)calloc(sizeof(struct vertex_t), R_MAX_VERTICES);
}

void r_Finish()
{

    glDeleteBuffers(1, &r_triangle_uniform_buffer);
    glDeleteBuffers(1, &r_spheres_uniform_buffer);
    glDeleteBuffers(1, &r_material_uniform_buffer);
    glDeleteBuffers(1, &r_vertex_uniform_buffer);


    glDeleteTextures(1, &r_rand_texture);


    free(r_spheres_buffer);
    free(r_material_buffer);
    free(r_triangle_buffer);
    free(r_vertex_buffer);
}

void r_Trace()
{
    int rand_offset_x;
    int rand_offset_y;

    glUseProgram(trace_test_shader->shader_program);
    glUniform1f(trace_test_shader->r_width, (float)window_width);
    glUniform1f(trace_test_shader->r_height, (float)window_height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, r_rand_texture);
    glUniform1i(trace_test_shader->r_rand_samples, 0);
    glUniform1i(trace_test_shader->r_max_rand_samples, R_RAND_SAMPLES);
    glUniform1i(trace_test_shader->r_samples, r_samples);


    //rand_offset_x = rand() >> 2;
    //rand_offset_y = (rand_offset_x + rand()) % RAND_MAX;

    //glUniform1i(trace_test_shader->r_rand_offset_x, rand_offset_x);
    //glUniform1i(trace_test_shader->r_rand_offset_y, rand_offset_y);



    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, r_rand_texture);
    //glUniform1i(trace_test_shader->r_rand_texture, 1);

    /*r_spheres_buffer[0].center_radius = (float [4]){0.0, -0.5, -8.0, 1.0};
    r_spheres_buffer[1].center_radius = (float [4]){0.0, -101.2, 0.0, 100.0};*/
    static float f = 0.0;
    static float r = 0.0;
/*
    static float pitch = 0.0;
    static float yaw = 0.0;*/

    r_spheres_buffer[0].center_radius[0] = 0.0;
    r_spheres_buffer[0].center_radius[1] = 0.9 + sin(f);
    r_spheres_buffer[0].center_radius[2] = 0.0;
    r_spheres_buffer[0].center_radius[3] = 1.4;
    r_spheres_buffer[0].material_index = 0;

    /*r_spheres_buffer[1].center_radius[0] = 0.0;
    r_spheres_buffer[1].center_radius[1] = -101.3;
    r_spheres_buffer[1].center_radius[2] = 0.0;
    r_spheres_buffer[1].center_radius[3] = 100.0;
    r_spheres_buffer[1].material_index = 1;*/

    r_spheres_buffer[1].center_radius[0] = 4.8;
    r_spheres_buffer[1].center_radius[1] = 0.5;
    r_spheres_buffer[1].center_radius[2] = 0.0;
    r_spheres_buffer[1].center_radius[3] = 1.0;
    r_spheres_buffer[1].material_index = 2;

    r_spheres_buffer[2].center_radius[0] = -2.8;
    r_spheres_buffer[2].center_radius[1] = 0.5;
    r_spheres_buffer[2].center_radius[2] = 2.0;
    r_spheres_buffer[2].center_radius[3] = 1.0;
    r_spheres_buffer[2].material_index = 3;

    /*r_spheres_buffer[3].center_radius[0] = 20.8;
    r_spheres_buffer[3].center_radius[1] = 40.5;
    r_spheres_buffer[3].center_radius[2] = 2.0;
    r_spheres_buffer[3].center_radius[3] = 8.0;
    r_spheres_buffer[3].material_index = 3;*/

    glUniform1i(trace_test_shader->r_spheres_count, 3);


    r_material_buffer[0].base = vec4_t(0.0, 1.0, 0.0, 1.0);
    r_material_buffer[0].roughness = 0.0;
    r_material_buffer[0].type = MAT_MATERIAL_TYPE_METAL;

    r_material_buffer[1].base = vec4_t(0.0, 1.0, 0.0, 1.0);
    r_material_buffer[1].type = MAT_MATERIAL_TYPE_LAMBERT;

    r_material_buffer[2].base = vec4_t(1.0, 0.0, 0.0, 1.0);
    r_material_buffer[2].type = MAT_MATERIAL_TYPE_DIELECTRIC;
    r_material_buffer[2].refrection = 1.2;

    r_material_buffer[3].base = vec4_t(1.0, 0.0, 0.0, 1.0);
    r_material_buffer[3].type = MAT_MATERIAL_TYPE_LAMBERT;
    r_material_buffer[3].refrection = 1.2;

    r_material_buffer[4].base = vec4_t(0.0, 0.0, 1.0, 1.0);
    r_material_buffer[4].type = MAT_MATERIAL_TYPE_LAMBERT;
    r_material_buffer[4].refrection = 1.2;

    r_material_buffer[5].base = vec4_t(0.0, 0.0, 1.0, 1.0);
    r_material_buffer[5].roughness = 0.2;
    r_material_buffer[5].type = MAT_MATERIAL_TYPE_METAL;
    r_material_buffer[5].refrection = 1.2;


    #define FLOOR_SIZE 50.0
    #define FLOOR_Y -1.5

    r_vertex_buffer[0].position = vec4_t(0.0, 6.0, 3.0, 0.0);
    r_vertex_buffer[0].normal = vec4_t(0.0, 0.0, -1.0, 0.0);
    r_vertex_buffer[1].position = vec4_t(-5.0, 1.5, 3.0, 0.0);
    r_vertex_buffer[1].normal = vec4_t(0.0, 0.0, -1.0, 0.0);
    r_vertex_buffer[2].position = vec4_t(5.0, 1.5, 3.0, 0.0);
    r_vertex_buffer[2].normal = vec4_t(0.0, 0.0, -1.0, 0.0);

    r_vertex_buffer[3].position = vec4_t(-FLOOR_SIZE, FLOOR_Y, -FLOOR_SIZE, 0.0);
    r_vertex_buffer[3].normal = vec4_t(0.0, 1.0, 0.0, 0.0);
    r_vertex_buffer[4].position = vec4_t(-FLOOR_SIZE, FLOOR_Y, FLOOR_SIZE, 0.0);
    r_vertex_buffer[4].normal = vec4_t(0.0, 1.0, 0.0, 0.0);
    r_vertex_buffer[5].position = vec4_t(FLOOR_SIZE, FLOOR_Y, FLOOR_SIZE, 0.0);
    r_vertex_buffer[5].normal = vec4_t(0.0, 1.0, 0.0, 0.0);

    r_vertex_buffer[6].position = vec4_t(FLOOR_SIZE, FLOOR_Y, FLOOR_SIZE, 0.0);
    r_vertex_buffer[6].normal = vec4_t(0.0, 1.0, 0.0, 0.0);
    r_vertex_buffer[7].position = vec4_t(FLOOR_SIZE, FLOOR_Y, -FLOOR_SIZE, 0.0);
    r_vertex_buffer[7].normal = vec4_t(0.0, 1.0, 0.0, 0.0);
    r_vertex_buffer[8].position = vec4_t(-FLOOR_SIZE, FLOOR_Y, -FLOOR_SIZE, 0.0);
    r_vertex_buffer[8].normal = vec4_t(0.0, 1.0, 0.0, 0.0);





    r_triangle_buffer[0].vertices[0] = 0;
    r_triangle_buffer[0].vertices[1] = 1;
    r_triangle_buffer[0].vertices[2] = 2;
    r_triangle_buffer[0].material_index = 5;


    r_triangle_buffer[1].vertices[0] = 3;
    r_triangle_buffer[1].vertices[1] = 4;
    r_triangle_buffer[1].vertices[2] = 5;
    r_triangle_buffer[1].material_index = 1;


    r_triangle_buffer[2].vertices[0] = 6;
    r_triangle_buffer[2].vertices[1] = 7;
    r_triangle_buffer[2].vertices[2] = 8;
    r_triangle_buffer[2].material_index = 1;


    glUniform1i(trace_test_shader->r_triangles_count, 3);

    f += 0.09;
    //r += 0.02;

    glBindBuffer(GL_UNIFORM_BUFFER, r_spheres_uniform_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct sphere_t) * R_MAX_SPHERES, r_spheres_buffer);

    glBindBuffer(GL_UNIFORM_BUFFER, r_material_uniform_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct material_data_t) * MAT_MAX_MATERIALS, r_material_buffer);

    glBindBuffer(GL_UNIFORM_BUFFER, r_triangle_uniform_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct triangle_t) * R_MAX_TRIANGLES, r_triangle_buffer);

    glBindBuffer(GL_UNIFORM_BUFFER, r_vertex_uniform_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct vertex_t) * R_MAX_VERTICES, r_vertex_buffer);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, R_SPHERE_UNIFORM_BLOCK_BINDING, r_spheres_uniform_buffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, R_MATERIAL_UNIFORM_BLOCK_BINDING, r_material_uniform_buffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, R_TRIANGLE_UNIFORM_BLOCK_BINDING, r_triangle_uniform_buffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, R_VERTICE_UNIFORM_BLOCK_BINDING, r_vertex_uniform_buffer);

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










