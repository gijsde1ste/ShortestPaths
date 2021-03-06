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

struct DegreeThreeNode
{
    int id, nodeId;
    Point_2 vertex;
    int leftChild, rightChild, parentNode;
    // Used to keep track how many left/right childs a node has, to determine which deques to use during funnel splits
    int nLeftChilds, nRightChilds;
    Point_2 parent;
    int minDeque, maxDeque, deque, indexId;
};

class SparseShortestPathTree;

#include <SparseShortestPathTree.h>


class Triangulation
{
    public:
        Triangulation(double blockFactor = 1.0);
        void open(std::string file);
        std::vector<DegreeThreeNode> openUserData();
        void close();
        Node getNode(int index, bool inOrOut = false);
        Node getRoot();
        Edge getNextEdge();
        Edge commonEdge(Node a, Node b);
        void createPath(Point_2 target);
        Point_2 getRandomPoint();
        bool containsPoint(Point_2 points[3], Point_2 p);
        std::vector<int> containsPoint(Point_2 points[3], std::vector<Point_2> targets);
        bool finished();
        std::vector<Triangle> copyPolygon();
        void setPathProgress(int progress);
        std::vector<DegreeThreeNode> SplitVertices;

    protected:
        tpie::uncompressed_stream<Node> in;
        tpie::uncompressed_stream<Node> out;
        std::vector<DegreeThreeNode> degree3Nodes;
        std::vector<std::tuple<int, Point_2, DegreeThreeNode*, int>> treeStack;
        std::vector<std::vector<int>> path;
        int pathProgress;

    private:

};

#endif // TRIANGULATION_H
