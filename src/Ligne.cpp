#include "Ligne.h"

Ligne::Ligne(uint32_t nbEdge) : Geometry()
{
    m_vertices = (float*)malloc(6 * nbEdge * sizeof(float));
    m_uvs = (float*)malloc(4 * nbEdge * sizeof(float));
    m_normals = (float*)malloc(6 * nbEdge * sizeof(float));
    m_nbVertices = 2 * nbEdge;

    for (uint32_t i = 0; i < nbEdge; i++)
    {

        float pos[] = { (float)cos(i * 2 * M_PI / nbEdge), 0.0f, (float)sin(i * 2 * M_PI / nbEdge),
                        (float)cos((i + 1) * 2 * M_PI / nbEdge), 0.0f , (float)sin((i + 1) * 2 * M_PI / nbEdge) };


        for (uint32_t j = 0; j < 6; j++)
        {
            m_vertices[6 * i + j] = 0.5 * pos[j];
        }
        for (uint32_t j = 0; j < 2; j++)
            for (uint32_t k = 0; k < 1; k++)
                m_uvs[4 * i + 2 * j + k] = m_vertices[6 * i + 3 * j + k] + 0.5;

        for (uint32_t j = 0; j < 3; j++)
        {
            float normal[] = { 0.0, 0.0, 1.0 };
            for (uint32_t k = 0; k < 2; k++)
                m_normals[6 * i + 2 * j + k] = normal[k];
        }
    }
}