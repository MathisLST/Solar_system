#ifndef SKYBOX_H
#define SKYBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include <vector>

class Skybox
{

private:
    GLuint vbo;
    std::vector<const GLchar*> faces;
	GLuint cubemapTexture;

    float skyboxVertices[36*3] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    GLuint loadCubemap(std::vector<const char*> faces)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);


        for (unsigned int i = 0; i < faces.size(); i++)
        {
            SDL_Surface* data = IMG_Load(faces[i]);
            SDL_Surface* rgbData = SDL_ConvertSurfaceFormat(data, SDL_PIXELFORMAT_RGBA32, 0);
            SDL_FreeSurface(data);


            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, rgbData->w, rgbData->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rgbData->pixels);

        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

public:
    Skybox(const GLchar* front, const GLchar* left, const GLchar* back, const GLchar* right, const GLchar* upper, const GLchar* lower)
    {
        this->faces.push_back(front);
        this->faces.push_back(left);
        this->faces.push_back(back);
        this->faces.push_back(right);
        this->faces.push_back(upper);
        this->faces.push_back(lower);

        this->cubemapTexture = loadCubemap(faces);

        glGenBuffers(1, &this->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(this->skyboxVertices), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->skyboxVertices), this->skyboxVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Skybox(std::vector<const GLchar*> faces)
    {
        this->faces = faces;
        this->cubemapTexture = loadCubemap(faces);

        glGenBuffers(1, &this->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(this->skyboxVertices), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->skyboxVertices), this->skyboxVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void draw(Shader* shader, Camera camera, glm::mat4 projectionMatrix)
    {
        glDepthMask(GL_FALSE);
        glUseProgram(shader->getProgramID());
        {
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

            GLuint vSkyPosition = glGetAttribLocation(shader->getProgramID(), "vSkyPosition");
            glVertexAttribPointer(vSkyPosition, 3, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray(vSkyPosition);

            GLuint view = glGetUniformLocation(shader->getProgramID(), "view");
            glm::mat4 cameraMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(cameraMatrix));

            GLuint projection = glGetUniformLocation(shader->getProgramID(), "projection");
            glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubemapTexture);
            GLuint uSkybox = glGetUniformLocation(shader->getProgramID(), "uSkybox");
            glUniform1i(this->cubemapTexture, 0);

            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
        glUseProgram(0);
        glDepthMask(GL_TRUE);
    }
};

#endif