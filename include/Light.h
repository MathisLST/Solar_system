#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
private:
    glm::vec3 position;
    glm::vec3 ambiant;
    glm::vec3 diffuse;
    glm::vec3 specular;

public:

    Light(glm::vec3 position, glm::vec3 ambiant, glm::vec3 diffuse, glm::vec3 specular)
    {
        this->position = position;
        this->ambiant = ambiant;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    // getters
    glm::vec3 getPosition()
    {
        return this->position;
    }
    glm::vec3 getAmbiant()
    {
        return this->ambiant;
    }
    glm::vec3 getDiffuse()
    {
        return this->diffuse;
    }
    glm::vec3 getSpecular()
    {
        return this->specular;
    }
};

#endif
