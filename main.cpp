#include "main.h"

#include "vector.h"
#include "matrix.h"

SDL_Window *window;
SDL_GLContext context;


int window_width = 1366;
int window_height = 768;

extern int r_samples;


int main(int argc, char *argv[])
{

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("ERROR: SDL didn't init: Error cause:\n%s\n", SDL_GetError());
        return -1;
    }

    mat3_t pitch_matrix;
    mat3_t yaw_matrix;

    float pitch = 0.0;
    float yaw = 0.0;

    int x = window_width / 2;
    int y = window_height / 2;

    float dx;
    float dy;

    struct camera_t *camera;

    int run = 1;
    SDL_Event events;
    const Uint8 *keys;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);


    window = SDL_CreateWindow("gpurt", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);

    GLenum init_status = glewInit();

    if(init_status != GLEW_NO_ERROR)
    {
        printf("ERROR: GLEW didn't init: Error cause:\n%s\n", glewGetErrorString(init_status));
        return -1;
    }

    camera = camera_CreateCamera(vec3_t(0.0, 0.0, 0.0), mat3_t(vec3_t(1.0, 0.0, 0.0), vec3_t(0.0, 1.0, 0.0), vec3_t(0.0, 0.0, 1.0)));

    r_SetActiveCamera(camera);

    r_Init();
    //shd_LoadShader("shader.glsl");

    while(run)
    {
        while(SDL_PollEvent(&events))
        {
            switch(events.type)
            {
                case SDL_KEYDOWN:

                    switch(events.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            run = 0;
                        break;

                        case SDLK_r:
                            if(r_samples < 64)
                            {
                                r_samples++;
                            }

                            printf("samples per pixel: %d\n", r_samples);

                        break;

                        case SDLK_t:
                            if(r_samples > 1)
                            {
                                r_samples--;
                            }

                            printf("samples per pixel: %d\n", r_samples);
                        break;

                    }

                    if(events.key.keysym.sym == SDLK_ESCAPE)
                    {
                        run = 0;
                    }

                break;
            }
        }

        SDL_GetMouseState(&x, &y);
        dx = (float)(window_width / 2) - (float)x;
        dy = (float)(window_height / 2) - (float)y;


        yaw -= dx * 0.001;
        pitch += dy * 0.001;

        if(pitch > 0.5) pitch = 0.5;
        else if(pitch < -0.5) pitch = -0.5;

        SDL_WarpMouseInWindow(window, window_width / 2, window_height / 2);

        pitch_matrix.identity();
        yaw_matrix.identity();

        pitch_matrix.rotate_x(pitch, 1);
        yaw_matrix.rotate_y(yaw, 1);

        camera->orientation = pitch_matrix * yaw_matrix;





        keys = SDL_GetKeyboardState(NULL);

        if(keys[SDL_SCANCODE_W])
        {
            camera->position -= camera->orientation[2];
        }
        if(keys[SDL_SCANCODE_S])
        {
            camera->position += camera->orientation[2];
        }



        if(keys[SDL_SCANCODE_A])
        {
            camera->position -= camera->orientation[0];
        }
        if(keys[SDL_SCANCODE_D])
        {
            camera->position += camera->orientation[0];
        }






        r_TestTrace();

        SDL_GL_SwapWindow(window);
    }



    return 0;
}







