//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>


#include "Camera.h"

#include "Shader.h"

#include "Planet.h"
#include "Skybox.h"
#include "Light.h"
#include "Orbites.h"

#include "logger.h"
#include <cmath>
#include <math.h>
#include <stack>
#include <vector>



#define WIDTH     1920
#define HEIGHT    1080
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))











int main(int argc, char* argv[])
{
    ////////////////////////////////////////
    //SDL2 / OpenGL Context initialization : 
    ////////////////////////////////////////

    //Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
        return 0;
    }

    //Initialize OpenGL Version (version 3.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    //Create a Window
    SDL_Window* window = SDL_CreateWindow("Solar System",                           //Titre
        SDL_WINDOWPOS_UNDEFINED,               //X Position
        SDL_WINDOWPOS_UNDEFINED,               //Y Position
        WIDTH, HEIGHT,                         //Resolution
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); //Flags (OpenGL + Show)
    SDL_SetRelativeMouseMode(SDL_TRUE);

    //Initialize the OpenGL Context (where OpenGL resources (Graphics card resources) lives)
    SDL_GLContext context = SDL_GL_CreateContext(window);

    //Tells GLEW to initialize the OpenGL function with this version
    glewExperimental = GL_TRUE;
    glewInit();


    //Start using OpenGL to draw something on screen
    glViewport(0, 0, WIDTH, HEIGHT); //Draw on ALL the screen

    //The OpenGL background color (RGBA, each component between 0.0f and 1.0f)
    glClearColor(0.0, 0.0, 0.0, 1.0); //Full Black

    glEnable(GL_DEPTH_TEST); //Active the depth test
    glEnable(GL_LIGHTING);


    // CAMERA
    Camera camera(glm::vec3(0.0f));
    camera.Reset(glm::vec3(0.0f, 5.0f, 13.0f));
    float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;

    // dit au système si c'est la première position ou non
    bool firstMouse = true;

    // timing
    float deltaTime = 0.0f;    // temps entre la frame actuelle et la dernière
    float lastFrame = 0.0f;    // temps de la dernière frame

    FILE* vertFile = fopen("Shaders/color.vert", "r");
    FILE* fragFile = fopen("Shaders/color.frag", "r");
    Shader* shader = Shader::loadFromFiles(vertFile, fragFile);

    FILE* skyVertFile = fopen("Shaders/skybox.vert", "r");
    FILE* skyFragFile = fopen("Shaders/skybox.frag", "r");
    Shader* skyboxShader = Shader::loadFromFiles(skyVertFile, skyFragFile);

    FILE* sunVertFile = fopen("Shaders/sun.vert", "r");
    FILE* sunFragFile = fopen("Shaders/sun.frag", "r");
    Shader* sunShader = Shader::loadFromFiles(sunVertFile, sunFragFile);



    Light light(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.3f, 0.3f, 0.3f));

    std::vector<const GLchar*> faces = { "textures/Cstars.png","textures/Cmilkyway.png","textures/Cstars.png","textures/Cstars.png","textures/Cstars.png","textures/Cstars.png" };

    Skybox skybox = Skybox(faces);


    Sun Sun("Sun", 1.0f, sunShader, "textures/2k_sun.png");

    //                  name          size   dist  shader     texture principale            texture speculaire     brillance  orbite  rota ang
    Planet Mercury   ("Mercury",      0.1f,  1.0f, shader, "textures/2k_mercury.png",                                 4.0f,  0.008f,   0.0f);
    Planet Venus     ("Venus",        0.2f,  2.0f, shader, "textures/2k_venus_atmosphere.png",                        4.0f,  0.004f,   0.0f);
    Planet Earth     ("Earth",        0.3f,  3.0f, shader, "textures/2k_earth.png", "textures/2k_earth_specular.png", 16.0f, 0.003f,   0.03f);
    Planet Mars      ("Mars",         0.15f, 4.0f, shader, "textures/2k_mars.png",                                    4.0f,  0.001f,   0.03f);
    Planet Jupiter   ("Jupiter",      0.8f,  5.0f, shader, "textures/2k_jupiter.png",                                 4.0f,  0.0005f,  0.015f);
    Planet Saturn    ("Saturn",       0.7f,  6.0f, shader, "textures/2k_saturn.png",                                  4.0f,  0.00025f, 0.03f);
    Planet Uranus    ("Uranus",       0.5f,  7.0f, shader, "textures/2k_uranus.png",                                  4.0f,  0.00018f, 0.00f);
    Planet Neptune   ("Neptune",      0.5f,  8.0f, shader, "textures/2k_neptune.png",                                 4.0f,  0.00009f, 0.03f);
    Planet Earth_moon("Earth's moon", 0.2f,  0.8f, shader, "textures/2k_moon.png",                                    4.0f,  0.1f,     0.00f);
    
    Earth.addChildren(&Earth_moon);

    std::vector<Planet*> SunChildren = { &Mercury, &Venus, &Earth, &Mars, &Jupiter, &Saturn, &Uranus, &Neptune };
    Sun.addChildren(SunChildren);



    Orbite orbiteMercury(Mercury.getDistance() * 2);
    Orbite orbiteVenus(Venus.getDistance() * 2);
    Orbite orbiteEarth(Earth.getDistance() * 2);
    Orbite orbiteMars(Mars.getDistance() * 2);
    Orbite orbiteJupiter(Jupiter.getDistance() * 2);
    Orbite orbiteSaturn(Saturn.getDistance() * 2);
    Orbite orbiteUranus(Uranus.getDistance() * 2);
    Orbite orbiteNeptune(Neptune.getDistance() * 2);


    std::vector<Orbite*> orbites;
    orbites.push_back(&orbiteMercury);
    orbites.push_back(&orbiteVenus);
    orbites.push_back(&orbiteEarth);
    orbites.push_back(&orbiteMars);
    orbites.push_back(&orbiteJupiter);
    orbites.push_back(&orbiteSaturn);
    orbites.push_back(&orbiteUranus);
    orbites.push_back(&orbiteNeptune);

    bool isOpened = true;

    //Main application loop
    while (isOpened)
    {
        //Time in ms telling us when this frame started. Useful for keeping a fix framerate
        uint32_t timeBegin = SDL_GetTicks();

        deltaTime = timeBegin - lastFrame;
        lastFrame = timeBegin;

        //Fetch the SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    isOpened = false;
                    break;
                default:
                    break;
                }
                break;
                //We can add more event, like listening for the keyboard or the mouse. See SDL_Event documentation for more details
            case SDL_MOUSEMOTION:
                camera.MotionMouse(event.motion.xrel, event.motion.yrel);
                break;

            case SDL_MOUSEWHEEL:
                camera.MotionScroll(event.wheel.y);
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    isOpened = false;
                    break;
                case SDLK_z:
                    camera.MotionKeyboard(FORWARD, deltaTime);
                    break;
                case SDLK_q:
                    camera.MotionKeyboard(LEFT, deltaTime);
                    break;
                case SDLK_s:
                    camera.MotionKeyboard(BACKWARD, deltaTime);
                    break;
                case SDLK_d:
                    camera.MotionKeyboard(RIGHT, deltaTime);
                    break;
                case SDLK_SPACE:
                    camera.MotionKeyboard(UP, deltaTime);
                    break;
                case SDLK_LCTRL:
                    camera.MotionKeyboard(DOWN, deltaTime);
                    break;
                case SDLK_a:
                    Sun.rotating = !Sun.rotating;
                    break;
                case SDLK_e:
                    for (Orbite* o : orbites)
                    {
                        o->affiche = !o->affiche;
                    }
                    break;
                case SDLK_r:
                    camera.Reset(glm::vec3(0.0f, 5.0f, 13.0f));
                    break;
                default:
                    break;
                }

            }
        }

        //Clear the screen : the depth buffer and the color buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 cameraMatrix = camera.GetViewMatrix();
        std::stack<glm::mat4> vpStack;
        vpStack.push(projectionMatrix * cameraMatrix);

        // on dessine la skybox
        skybox.draw(skyboxShader, camera, projectionMatrix);

        // on dessine les orbites
        for (Orbite* o : orbites) {
            o->draw(sunShader, vpStack);
        }

        // on dessine le Soleil et les planètes
        Sun.draw(vpStack, Sun, light, camera);

        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if (timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
    }

    //Free everything

    if (context != NULL)
        SDL_GL_DeleteContext(context);
    if (window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}