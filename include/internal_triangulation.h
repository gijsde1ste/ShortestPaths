#ifndef INTERNAL_TRIANGULATION_H
#define INTERNAL_TRIANGULATION_H

#include <definition.h>
#include <Triangulation.h>
#include <time.h>

class internal_triangulation
{
    public:
        internal_triangulation();
        void open(std::string file);
        Node getRoot();
        Edge getNextEdge();
        Edge commonEdge(Node a, Node b);
        void createPath(Point_2 target);
        Point_2 getRandomPoint();
        bool containsPoint(Point_2 points[3], Point_2 p);
        bool finished();
        std::vector<Triangle> copyPolygon();
        size_t size();

    protected:

    private:
        tpie::uncompressed_stream<Node> in;
        std::vector<Node> triangulation;
        std::vector<int> path;
        int pathProgress;
};

#endif // INTERNAL_TRIANGULATION_H
