#ifndef SPARSESHORTESTPATHTREE_H
#define SPARSESHORTESTPATHTREE_H

#include <base_funnel.h>
#include <Triangulation.h>
#include <deque.h>

struct ssptResultPoint {
    Point_2 point;
    int parent;
};

class SparseShortestPathTree: public base_funnel
{
    public:
        SparseShortestPathTree(std::vector<DegreeThreeNode> degreeThreeNodes);
        int assignNChilds(DegreeThreeNode * dtn);
        void assignDeques(DegreeThreeNode * dtn, int min, int max);
        void assignFingerParents(DegreeThreeNode * dtn, Point_2 p, bool front);
        void setCurrentDtn(DegreeThreeNode * dtn);
        void extendStart(Node source, Edge e);
        void extend(Edge e);
        void extend(Point_2 p, bool front);
        void extendFinalStep(Point_2 target);
        int getCuspCount();
        int getDeque(DegreeThreeNode * dtn);
        Point_2 peekFront();
        Point_2 peekBack();
        Point_2 popFront();
        Point_2 popBack();

        int outputCounter, outputParent;
        tpie::uncompressed_stream<ssptResultPoint> out;

        std::vector<tpie::deque<Point_2>*> cusps;
        tpie::deque<Point_2> * cusp;
        int cuspCount;
        int currentCusp;
        int minCusp, maxCusp;
        bool cuspChanged;
        DegreeThreeNode * currentDtn;

        std::vector<DegreeThreeNode> degree3Nodes;
        Point_2 point2Zero;

    protected:

    private:




};

#endif // SPARSESHORTESTPATHTREE_H
