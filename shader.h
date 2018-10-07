#ifndef SHADER_H
#define SHADER_H





struct shader_t
{
    struct shader_t *next;
    struct shader_t *prev;

    unsigned int shader_program;
    unsigned int vertex_shader;
    unsigned int fragment_shader;


    //unsigned int spheres_array;
    unsigned int r_width;
    unsigned int r_height;

    unsigned int r_camera_position;
    unsigned int r_camera_orientation;

    unsigned int r_max_rand_samples;
    unsigned int r_rand_samples;

    unsigned int r_samples;


    unsigned int r_spheres_uniform_buffer;
    unsigned int r_spheres_count;

    char *shader_name;
};




struct shader_t *shd_LoadShader(const char *file_name);





#endif // SHADER_H
