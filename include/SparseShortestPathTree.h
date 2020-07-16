#ifndef SPARSESHORTESTPATHTREE_H
#define SPARSESHORTESTPATHTREE_H

#include <base_funnel.h>
#include <Triangulation.h>
#include <deque.h>

class SparseShortestPathTree: public base_funnel
{
    public:
        SparseShortestPathTree();
        void extendStart(Node source, Edge e);
        void extend(Edge e);
        void extend(Point_2 p, bool front);
        void extendFinalStep(Point_2 target);
        void startNextDeque();
        void startNextStack();
        int getCuspCount();
        int getStackCount();

        std::vector<tpie::deque<Point_2>*> cusps;
        tpie::deque<Point_2> * cusp;
        int cuspCount;

        std::vector<tpie::stack<Point_2>*> stacks;
        tpie::stack<Point_2> * geodesic;
        int stackCount;

    protected:

    private:
        std::vector<int> degree3Nodes;


};

#endif // SPARSESHORTESTPATHTREE_H
