#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <definition.h>
#include <time.h>
#include <tpie/internal_stack.h>

struct Node
{
    Point_2 points[3];
    int id, parent;
    int leftChild, rightChild;
    int postOrder, preOrder;
};

struct TraversalInfo
{
    int NodeId;
    int CuspId;
    int StackId;
};

class SparseShortestPathTree;

#include <SparseShortestPathTree.h>


class Triangulation
{
    public:
        Triangulation(double blockFactor = 1.0);
        void open(std::string file);
        std::vector<int> openUserData();
        void close();
        Node getNode(int index);
        Node getRoot();
        Edge getNextEdge(bool shortestPathTree = false, SparseShortestPathTree * sspt = nullptr);
        Edge commonEdge(Node a, Node b);
        void createPath(Point_2 target);
        void createPath(std::vector<Point_2> targets);
        Point_2 getRandomPoint();
        bool containsPoint(Point_2 points[3], Point_2 p);
        std::vector<int> containsPoint(Point_2 points[3], std::vector<Point_2> targets);
        bool finished(bool shortestPathTree = false);
        std::vector<Triangle> copyPolygon();
        void setPathProgress(int progress);

    protected:

    private:
        tpie::uncompressed_stream<Node> in;
        std::vector<TraversalInfo> degree3Nodes;
        std::vector<std::vector<int>> path;
        int pathProgress;
};

#endif // TRIANGULATION_H
