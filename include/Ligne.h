#ifndef  LIGNE_INC
#define  LIGNE_INC

#include "Geometry.h"
#include <cmath>

class Ligne : public Geometry
{
public:
    /* \brief Create a circle
     * \param nbEdges the number of edges for this circle */
    Ligne(uint32_t nbEdges);
};

#endif