#ifndef SSPTTRIANGULATION_H
#define SSPTTRIANGULATION_H

#include <Triangulation.h>

class SsptTriangulation: public Triangulation
{
    public:
        bool finished();
        Edge getNextEdge(SparseShortestPathTree * sspt);
        void createPath(std::vector<Point_2> targets);

        std::vector<DegreeThreeNode> tempNodes;

    protected:

    private:
};

#endif // SSPTTRIANGULATION_H
