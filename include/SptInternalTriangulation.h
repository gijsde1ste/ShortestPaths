#ifndef SPTINTERNALTRIANGULATION_H
#define SPTINTERNALTRIANGULATION_H

#include <internal_triangulation.h>

class SptInternalTriangulation: public internal_triangulation
{
    public:
        Edge getNextEdge();
        bool finished();

    protected:

    private:
};

#endif // SPTINTERNALTRIANGULATION_H
