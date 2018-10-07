#include "shader.h"

#include "GL/glew.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static const char version[] = "#version 400 compatibility\n";
static const int version_len = sizeof(version) - 1;
static const char define[] = "#define VERTEX_SHADER\n";
static const int define_len = sizeof(define) - 1;

struct shader_t *shd_shaders = NULL;
struct shader_t *shd_last_shader = NULL;

struct shader_t *shd_LoadShader(const char *file_name)
{
    FILE *file;
    unsigned long file_len;

    int i;
    int info_log_len;

    char *file_buffer;
    char *info_log;
    //char *vertex_shader_source;
    //char *fragment_shader_source;

    int compile_status;
    int link_status;

//    int tag_len;
//   char tag[128];

    struct shader_t *shader = NULL;

    file = fopen(file_name, "rb");

    if(file)
    {
        fseek(file, 0, SEEK_END);
        file_len = ftell(file);
        rewind(file);

        file_buffer = (char *)calloc(file_len + 1 + define_len + version_len, 1);
        fread(file_buffer + define_len + version_len, file_len, 1, file);
        fclose(file);
        file_buffer[define_len + file_len + version_len] = '\0';

        for(i = 0; i < version_len + 1; i++)
        {
            file_buffer[i] = version[i];
        }

        for(i = 0; i < define_len; i++)
        {
            file_buffer[i + version_len] = ' ';
        }

        shader = (struct shader_t *)calloc(sizeof(struct shader_t), 1);


        if(!shd_shaders)
        {
            shd_shaders = shader;
        }
        else
        {
            shd_last_shader->next = shader;
            shader->prev = shd_last_shader;
        }

        shd_last_shader = shader;


        shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        shader->shader_program = glCreateProgram();







        /* #define VERTEX_SHADER... */
        for(i = 0; i < define_len; i++)
        {
            file_buffer[i + version_len] = define[i];
        }

        //printf("%s\n", file_buffer);

        /* vertex shader... */
        glShaderSource(shader->vertex_shader, 1, &file_buffer, NULL);
        glCompileShader(shader->vertex_shader);
        glGetShaderiv(shader->vertex_shader, GL_COMPILE_STATUS, &compile_status);

        if(compile_status == GL_FALSE)
        {
            glGetShaderiv(shader->vertex_shader, GL_INFO_LOG_LENGTH, &info_log_len);
            info_log = (char *)calloc(info_log_len, 1);
            glGetShaderInfoLog(shader->vertex_shader, info_log_len, NULL, info_log);
            printf("shader [%s] vertex shader compilation failed! Error cause:\n%s\n", file_name, info_log);
            free(info_log);
            free(file_buffer);
            glDeleteShader(shader->vertex_shader);
            glDeleteShader(shader->fragment_shader);
            glDeleteProgram(shader->shader_program);
            return NULL;
        }






        for(i = 0; i < define_len; i++)
        {
            file_buffer[i + version_len] = ' ';
        }

        //printf("%s\n", file_buffer);

        /* fragment shader... */
        glShaderSource(shader->fragment_shader, 1, &file_buffer, NULL);
        glCompileShader(shader->fragment_shader);
        glGetShaderiv(shader->fragment_shader, GL_COMPILE_STATUS, &compile_status);

        if(compile_status == GL_FALSE)
        {
            glGetShaderiv(shader->fragment_shader, GL_INFO_LOG_LENGTH, &info_log_len);
            info_log = (char *)calloc(info_log_len, 1);
            glGetShaderInfoLog(shader->fragment_shader, info_log_len, NULL, info_log);
            printf("shader [%s] fragment shader compilation failed! Error cause:\n%s\n", file_name, info_log);
            free(info_log);
            free(file_buffer);
            glDeleteShader(shader->vertex_shader);
            glDeleteShader(shader->fragment_shader);
            glDeleteProgram(shader->shader_program);
            return NULL;
        }





        glAttachShader(shader->shader_program, shader->vertex_shader);
        glAttachShader(shader->shader_program, shader->fragment_shader);
        glLinkProgram(shader->shader_program);
        glGetProgramiv(shader->shader_program, GL_LINK_STATUS, &link_status);

        if(link_status == GL_FALSE)
        {
            glGetProgramiv(shader->shader_program, GL_INFO_LOG_LENGTH, &info_log_len);
            info_log = (char *)calloc(info_log_len, 1);
            glGetProgramInfoLog(shader->shader_program, info_log_len, NULL, info_log);
            printf("shader [%s] linking failed! Error cause:\n%s\n", file_name, info_log);
            free(info_log);
            free(file_buffer);
            glDeleteShader(shader->vertex_shader);
            glDeleteShader(shader->fragment_shader);
            glDeleteProgram(shader->shader_program);
            return NULL;
        }

        shader->r_width = glGetUniformLocation(shader->shader_program, "r_width");
        shader->r_height = glGetUniformLocation(shader->shader_program, "r_height");
        shader->r_max_rand_samples = glGetUniformLocation(shader->shader_program, "r_max_rand_samples");
        shader->r_rand_samples = glGetUniformLocation(shader->shader_program, "r_rand_samples");

        shader->r_camera_position = glGetUniformLocation(shader->shader_program, "r_camera_position");
        shader->r_camera_orientation = glGetUniformLocation(shader->shader_program, "r_camera_orientation");

        shader->r_samples = glGetUniformLocation(shader->shader_program, "r_samples");


        shader->r_spheres_count = glGetUniformLocation(shader->shader_program, "r_sphere_count");
        shader->r_spheres_uniform_buffer = glGetUniformBlockIndex(shader->shader_program, "r_sphere_uniform_block");
        glUniformBlockBinding(shader->shader_program, shader->r_spheres_uniform_buffer, 0);
    }

    return shader;
}






