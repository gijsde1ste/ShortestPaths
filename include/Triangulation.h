#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <definition.h>
#include <time.h>

struct Node
{
    Point_2 points[3];
    int id, parent;
    int leftChild, rightChild;
    int postOrder, preOrder;
};

class Triangulation
{
    public:
        Triangulation();
        void open(std::string file);
        void close();
        Node getNode(int index);
        Node getRoot();
        Edge getNextEdge();
        Edge commonEdge(Node a, Node b);
        void createPath(Point_2 target);
        Point_2 getRandomPoint();
        bool containsPoint(Point_2 points[3], Point_2 p);
        bool finished();
        std::vector<Triangle> copyPolygon();

    protected:

    private:
        tpie::uncompressed_stream<Node> in;
        std::vector<int> path;
        int pathProgress;
};

#endif // TRIANGULATION_H
