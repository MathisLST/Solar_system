#ifndef ORBITES_H
#define ORBITES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Ligne.h"

#include <stack>

#define INDICE_TO_PTR(x) ((void*)(x))

class Orbite
{

private:
	GLuint vbo;
	int nbVertices;
	glm::mat4 modelMatrix;
	GLuint texture;
	

    GLint generateVBO(const Geometry& geometry)
    {
        GLuint vboID;
        glGenBuffers(1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, (2 + 3 + 3) * sizeof(float) * geometry.getNbVertices(), NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * geometry.getNbVertices(), geometry.getVertices());
        glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * geometry.getNbVertices(), 3 * sizeof(float) * geometry.getNbVertices(), geometry.getNormals());
        glBufferSubData(GL_ARRAY_BUFFER, (3 + 3) * sizeof(float) * geometry.getNbVertices(), 2 * sizeof(float) * geometry.getNbVertices(), geometry.getUVs());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return vboID;
    }

    GLuint generateTexture(const char* filename) {
        SDL_Surface* image = IMG_Load(filename);
        SDL_Surface* rgbImage = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(image);

        GLuint textureName;
        glGenTextures(1, &textureName);
        glBindTexture(GL_TEXTURE_2D, textureName);
        {

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rgbImage->w, rgbImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rgbImage->pixels);

            glGenerateMipmap(GL_TEXTURE_2D);
        }
        return textureName;
    }


public:

    bool affiche;

    Orbite(float rayon, const char* filename = "textures/blanc.png")
    {
        Ligne ligne(64);
        this->vbo = generateVBO(ligne);
        this->nbVertices = ligne.getNbVertices();
        this->texture = generateTexture(filename);
        this->modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(rayon));
    }

    void draw(Shader* shader, std::stack<glm::mat4>& vpStack)
    {
        if (this->affiche)
        {
            glUseProgram(shader->getProgramID());
            {
                glBindBuffer(GL_ARRAY_BUFFER, this->vbo);


                GLuint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
                glVertexAttribPointer(vPosition, 3, GL_FLOAT, 0, 0, 0);
                glEnableVertexAttribArray(vPosition);

                GLuint vNormal = glGetAttribLocation(shader->getProgramID(), "vNormal");
                glVertexAttribPointer(vNormal, 3, GL_FLOAT, 0, 0, INDICE_TO_PTR(3 * sizeof(float) * this->nbVertices));
                glEnableVertexAttribArray(vNormal);

                GLuint vUV = glGetAttribLocation(shader->getProgramID(), "vUV");
                glVertexAttribPointer(vUV, 2, GL_FLOAT, 0, 0, INDICE_TO_PTR((3 + 3) * sizeof(float) * this->nbVertices));
                glEnableVertexAttribArray(vUV);

                GLuint uVP = glGetUniformLocation(shader->getProgramID(), "uVP");
                glUniformMatrix4fv(uVP, 1, GL_FALSE, glm::value_ptr(vpStack.top()));

                GLuint uPL = glGetUniformLocation(shader->getProgramID(), "uPL");
                glm::mat4 pl = this->modelMatrix;
                glUniformMatrix4fv(uPL, 1, GL_FALSE, glm::value_ptr(pl));

                // On allège la charge de travail du shader en envoyant directement l'inverse
                GLuint uPLinv = glGetUniformLocation(shader->getProgramID(), "uPLinv");
                glm::mat4 pl_inv = glm::inverse(pl);
                glUniformMatrix4fv(uPLinv, 1, GL_FALSE, glm::value_ptr(pl_inv));

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, this->texture);
                GLuint uDiffuse = glGetUniformLocation(shader->getProgramID(), "skin");
                glUniform1i(uDiffuse, 0);

                glDrawArrays(GL_LINE_STRIP, 0, this->nbVertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            glUseProgram(0);
        }
    }

};


#endif
