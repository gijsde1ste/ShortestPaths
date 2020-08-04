#ifndef SPARSESHORTESTPATHTREE_H
#define SPARSESHORTESTPATHTREE_H

#include <base_funnel.h>
#include <Triangulation.h>
#include <deque.h>

class SparseShortestPathTree: public base_funnel
{
    public:
        SparseShortestPathTree(std::vector<DegreeThreeNode> degreeThreeNodes);
        int assignNChilds(DegreeThreeNode * dtn);
        void assignFingerParents(DegreeThreeNode * dtn, Point_2 p, bool front);
        void extendStart(Node source, Edge e);
        void extend(Edge e);
        void extend(Point_2 p, bool front);
        void extendFinalStep(Point_2 target);
        int getCuspCount();
        int getStackCount();
        int getDeque(DegreeThreeNode * dtn);
        Point_2 peekFront();
        Point_2 peekBack();
        Point_2 popFront();
        Point_2 popBack();

        std::vector<tpie::deque<Point_2>*> cusps;
        tpie::deque<Point_2> * cusp;
        int cuspCount;
        int currentCusp;
        int minCusp, maxCusp;
        bool cuspChanged;
        DegreeThreeNode * currentDtn;

        std::vector<tpie::stack<Point_2>*> stacks;
        tpie::stack<Point_2> * geodesic;
        int stackCount;

        std::vector<DegreeThreeNode> degree3Nodes;

    protected:

    private:

        Point_2 point2Zero;


};

#endif // SPARSESHORTESTPATHTREE_H
