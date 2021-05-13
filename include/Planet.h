#ifndef PLANET_H
#define PLANET_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Sphere.h"
#include "Light.h"
#include "Camera.h"

#include <vector>
#include <stack>
#include <string>

#define INDICE_TO_PTR(x) ((void*)(x))

class Planet
{
private:
    std::string name;
    float size;
    float distance;
    Shader* shader;
    GLuint vbo;
    int nbVertices;
    glm::mat4 propagatedMatrix = glm::mat4(1.0f);
    glm::mat4 localMatrix = glm::mat4(1.0f);
    std::vector<Planet*> children;
    GLuint texture;
    GLuint texture_specular;
    float shininess;
    float rotation;
    float selfRotation;
    float deltaRotation;
    float deltaSelfRotation;
    bool twoTextures;   // true si la planète a une specular_map, false sinon


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

protected:
    // Permet de générer une texture
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

    // Constructeur
    Planet(std::string name, float taille, float distance, Shader* shader, const char* filename, float shininess, float deltaRota, float deltaSelfRota, bool twoTextures = false)
    {
        Sphere sphere(32, 32);
        this->name = name;
        this->size = taille;
        this->shader = shader;
        this->vbo = generateVBO(sphere);
        this->nbVertices = sphere.getNbVertices();
        this->children = {};
        this->texture = generateTexture(filename);
        this->shininess = shininess;
        this->deltaRotation = deltaRota;
        this->deltaSelfRotation = deltaSelfRota;
        this->twoTextures = twoTextures;
        this->distance = distance;
        this->rotation = 0.0f;
        this->selfRotation = 0.0f;

    }

    // Constructeur deux textures
    Planet(std::string name, float taille, float distance, Shader* shader, const char* filename, const char* filename_specular,float shininess, float deltaRota, float deltaSelfRota, bool twoTextures = true)
    {
        Sphere sphere(32, 32);
        this->name = name;
        this->size = taille;
        this->shader = shader;
        this->vbo = generateVBO(sphere);
        this->nbVertices = sphere.getNbVertices();
        this->children = {};
        this->texture = generateTexture(filename);
        this->texture_specular = generateTexture(filename_specular);
        this->shininess = shininess;
        this->deltaRotation = deltaRota;
        this->deltaSelfRotation = deltaSelfRota;
        this->twoTextures = twoTextures;
        this->rotation = 0.0f;
        this->selfRotation = 0.0f;
        this->distance = distance;
    }

    // ajoute des planètes dépendantes à la planète en question via un vecteur en entrée
    void addChildren(std::vector<Planet*> children)
    {
        this->children = children;
    }

    // ajoute une planète dépendante à la planète en question
    void addChildren(Planet* child)
    {
        this->children.push_back(child);
    }

    void addSelfRotation()
    {
        this->selfRotation += deltaSelfRotation;
    }

    void addRotation()
    {
        this->rotation += deltaRotation;
    }

    // getters / setters
    std::string getName()
    {
        return name;
    }
    Shader* getShader()
    {
        return this->shader;
    }
    GLuint getVBO()
    {
        return this->vbo;
    }
    int getNbVertices()
    {
        return this->nbVertices;
    }
    glm::mat4 getPropagatedMatrix()
    {
        return this->propagatedMatrix;
    }
    void setPropagatedMatrix(glm::mat4 newPropa)
    {
        this->propagatedMatrix = newPropa;
    }
    void setLocalMatrix(glm::mat4 newLocal)
    {
        this->localMatrix = newLocal;
    }
    glm::mat4 getLocalMatrix()
    {
        return this->localMatrix;
    }
    std::vector<Planet*> getChildren()
    {
        return this->children;
    }
    GLuint getTexture()
    {
        return this->texture;
    }
    GLuint getTextureSpecular()
    {
        return this->texture_specular;
    }
    float getShininess()
    {
        return this->shininess;
    }
    float getDeltaRotation()
    {
        return this->deltaRotation;
    }
    float getDeltaSelfRotation()
    {
        return this->deltaSelfRotation;
    }
    float getRotation()
    {
        return this->rotation;
    }
    float getSelfRotation()
    {
        return this->selfRotation;
    }
    float getTwoTextures()
    {
        return this->twoTextures;
    }
    float getSize()
    {
        return this->size;
    }
    float getDistance()
    {
        return this->distance;
    }

};



// Sous classe planète
class Sun : public Planet
{
public:

    bool rotating;

    Sun(std::string name, float taille, Shader* shader, const char* filename)
        : Planet(name, taille, 0.0f, shader, filename, 0.0f, 0.0f, 0.0f) 
    {
        this->rotating = true;
    }

    // Dessine le système solaire
    void draw(std::stack<glm::mat4>& vpStack, Planet& planet, Light light, Camera camera)
    {
        
        // Rotation autour du Soleil
        glm::mat4 newPropa = glm::translate(glm::mat4(1.0f), glm::vec3(planet.getDistance() * sin(planet.getRotation()), 0.0f, planet.getDistance() * cos(planet.getRotation())))
            * glm::scale(glm::mat4(1.0f), glm::vec3(planet.getSize()));
        planet.setPropagatedMatrix(newPropa);

        glm::mat4 newLocal = glm::rotate(glm::mat4(1.0f), planet.getSelfRotation(), glm::vec3(0.0f, 1.0f, 0.0f));
        planet.setLocalMatrix(newLocal);

        // Maj rotation
        if (this->rotating)
        {
            planet.addRotation();
            planet.addSelfRotation();
        }
        
        glUseProgram(planet.getShader()->getProgramID());
        {
            glBindBuffer(GL_ARRAY_BUFFER, planet.getVBO());

            // on envoie les positions des sommets
            GLuint vPosition = glGetAttribLocation(planet.getShader()->getProgramID(), "vPosition");
            glVertexAttribPointer(vPosition, 3, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray(vPosition);

            // on envoie les normales
            GLuint vNormal = glGetAttribLocation(planet.getShader()->getProgramID(), "vNormal");
            glVertexAttribPointer(vNormal, 3, GL_FLOAT, 0, 0, INDICE_TO_PTR(3 * sizeof(float) * planet.getNbVertices()));
            glEnableVertexAttribArray(vNormal);

            // on envoie la map pour la texture 
            GLuint vUV = glGetAttribLocation(planet.getShader()->getProgramID(), "vUV");
            glVertexAttribPointer(vUV, 2, GL_FLOAT, 0, 0, INDICE_TO_PTR((3 + 3) * sizeof(float) * planet.getNbVertices()));
            glEnableVertexAttribArray(vUV);

            // on envoie la view/projection matrix
            GLuint uVP = glGetUniformLocation(planet.getShader()->getProgramID(), "uVP");
            glUniformMatrix4fv(uVP, 1, GL_FALSE, glm::value_ptr(vpStack.top()));

            // on envoie la propagated/local matrix
            GLuint uPL = glGetUniformLocation(planet.getShader()->getProgramID(), "uPL");
            glm::mat4 pl = planet.getPropagatedMatrix() * planet.getLocalMatrix();
            glUniformMatrix4fv(uPL, 1, GL_FALSE, glm::value_ptr(pl));

            // on allège la charge de travail du shader en envoyant directement l'inverse de la propagated/local
            GLuint uPLinv = glGetUniformLocation(planet.getShader()->getProgramID(), "uPLinv");
            glm::mat4 pl_inv = glm::inverse(pl);
            glUniformMatrix4fv(uPLinv, 1, GL_FALSE, glm::value_ptr(pl_inv));

            if (planet.getName() != "Sun")
            {
                // on envoie la texture principale
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, planet.getTexture());
                GLuint uDiffuse = glGetUniformLocation(planet.getShader()->getProgramID(), "planet.diffuse");
                glUniform1i(uDiffuse, 0);

                // on dit au shader si l'on a une ou deux textures
                GLuint uTwoTex = glGetUniformLocation(planet.getShader()->getProgramID(), "planet.two");
                glUniform1i(uTwoTex, (int) planet.getTwoTextures());

                if (planet.getTwoTextures())
                {
                    // on envoie la deuxième texture
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, planet.getTextureSpecular());
                    GLuint uSpecular = glGetUniformLocation(planet.getShader()->getProgramID(), "planet.specular");
                    glUniform1i(uSpecular, 1);
                }

                // on envoie le coefficient de brillance
                GLuint uShininess = glGetUniformLocation(planet.getShader()->getProgramID(), "planet.shininess");
                glUniform1f(uShininess, planet.getShininess());

                // on envoie la position de lumière
                GLuint uLightPosition = glGetUniformLocation(planet.getShader()->getProgramID(), "light.position");
                glUniform3fv(uLightPosition, 1, glm::value_ptr(light.getPosition()));

                // on envoie la composante ambiante de la lumière
                GLuint uLightAmbiant = glGetUniformLocation(planet.getShader()->getProgramID(), "light.ambiant");
                glUniform3fv(uLightAmbiant, 1, glm::value_ptr(light.getAmbiant()));

                // on envoie la composante diffuse de la lumière
                GLuint uLightDiffuse = glGetUniformLocation(planet.getShader()->getProgramID(), "light.diffuse");
                glUniform3fv(uLightDiffuse, 1, glm::value_ptr(light.getDiffuse()));

                // on envoie la composante spéculaire de la lumière
                GLuint uLightSpecular = glGetUniformLocation(planet.getShader()->getProgramID(), "light.specular");
                glUniform3fv(uLightSpecular, 1, glm::value_ptr(light.getSpecular()));

                // on envoie la position de la caméra
                GLuint uViewPosition = glGetUniformLocation(planet.getShader()->getProgramID(), "viewPosition");
                glUniform3fv(uViewPosition, 1, glm::value_ptr(camera.Position));
            }
            else
            {
                // on envoie la texture principale
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, planet.getTexture());
                GLuint uTexture = glGetUniformLocation(planet.getShader()->getProgramID(), "skin");
                glUniform1i(uTexture, 0);
            }
       
            glDrawArrays(GL_TRIANGLES, 0, planet.getNbVertices());
            vpStack.push(vpStack.top() * planet.getPropagatedMatrix());

            // on dessine les planètes enfants par récursivité
            for (Planet* child : planet.getChildren())
            {
                draw(vpStack, *child, light, camera);
            }
            vpStack.pop();
        }
        glUseProgram(0);
    }
};

#endif