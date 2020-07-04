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

// Used to save k leaf to root paths through the triangulation
struct pathNode
{
    int postOrder;
    int id;
    int leftChild, rightChild;
};

class Triangulation
{
    public:
        Triangulation(double blockFactor = 1.0);
        void open(std::string file);
        void close();
        Node getNode(int index);
        Node getRoot();
        Edge getNextEdge();
        Edge commonEdge(Node a, Node b);
        void createPath(Point_2 target);
        void createPath(std::vector<Point_2> targets);
        Point_2 getRandomPoint();
        bool containsPoint(Point_2 points[3], Point_2 p);
        std::vector<int> containsPoint(Point_2 points[3], std::vector<Point_2> targets);
        bool finished();
        std::vector<Triangle> copyPolygon();

    protected:

    private:
        tpie::uncompressed_stream<Node> in;
        std::vector<std::vector<int>> path;
        int pathProgress;
};

#endif // TRIANGULATION_H
