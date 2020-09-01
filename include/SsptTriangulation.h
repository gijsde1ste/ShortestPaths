#ifndef SSPTTRIANGULATION_H
#define SSPTTRIANGULATION_H

#include <Triangulation.h>

class SsptTriangulation: public Triangulation
{
    public:
        bool finished();
        Edge getNextEdge(SparseShortestPathTree * sspt);

    protected:

    private:
};

#endif // SSPTTRIANGULATION_H
