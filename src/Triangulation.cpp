#include "Triangulation.h"

Triangulation::Triangulation(double blockFactor)
    : in(blockFactor)
{
    pathProgress = 0;
    srand(time(NULL));
}

void Triangulation::open(std::string file)
{
    in.open(file);
}

void Triangulation::close()
{
    in.close();
}

Node Triangulation::getRoot()
{
    in.seek(-1, in.end);
    return in.read();
}

Node Triangulation::getNode(int index)
{
    in.seek(index);
    return in.read();
}

Point_2 Triangulation::getRandomPoint()
{
    Node source = getRoot();
    Point_2 result;
    while (true){
        int triangleIndex = rand() % in.size();
        int vertexIndex = rand() % 3;
        result = getNode(triangleIndex).points[vertexIndex];

        // if the random point we found is not part of source triangle
        if (!containsPoint(source.points, result)) break;
    }

    return result;
}

// Creates the path for a single target point
void Triangulation::createPath(Point_2 target){
    path.clear();
    pathProgress = 0;

    in.seek(0);
    Node n;
    // find a triangle containing target by scanning triangulation
    while (in.can_read()){
        n = in.read();
        if (containsPoint(n.points, target)) {
            // a target triangle has been found
            break;
        }
    }

    // construct the path from n to the source (root)
    // its possible we find a triangle closer to the root that also contains the target point
    std::vector<int> step;
    step.push_back(n.postOrder);
    path.push_back(step);
    while (n.parent != -1){
        n = getNode(n.parent);
        // Check if parent contains target point, if so this is a shorter path to the root
        if (containsPoint(n.points, target)) {
            path.clear();
        }
        step.clear();
        step.push_back(n.postOrder);
        path.push_back(step);
    }

    // Orient away from source
    std::reverse(path.begin(), path.end());
}

void Triangulation::createPath(std::vector<Point_2> targets){

    path.clear();
    pathProgress = 0;

    in.seek(0);
    Node n;
    Node targetTriangles[targets.size()];

    // find triangles containing targets by scanning triangulation
    while (in.can_read()){

        n = in.read();

        std::vector<int> foundTargets = containsPoint(n.points, targets);
        // The logic here is, if node n contains one of our target points we store it
        // If the postOrder number is bigger for the same target it is closer to the source point
        // Since input is sorted in postOrder always override previous value
        for (auto it = foundTargets.begin(); it != foundTargets.end(); ++it){
            targetTriangles[*it] = n;
        }
    }

    pathNode lastPathNodes[targets.size()];
    // Put the initial nodes in the output path
    for (int i = 0; i < targets.size(); ++i){
        pathNode node = {targetTriangles[i].postOrder, -1, -1, -1};
        lastPathNodes[i] = node;
    }

    tpie::uncompressed_stream<pathNode> out;
    out.open("leafToRoot.tpie", tpie::access_write);

    in.seek(0);
    int count = 0;
    bool skip = false;
    while (in.can_read()){
        n = in.read();
        for (int i = 0; i < targets.size(); ++i){
            if (n.leftChild == lastPathNodes[i].postOrder || n.rightChild == lastPathNodes[i].postOrder){
                // Check whether node n is already used in shortest path tree
                for (int j = 0; j < targets.size(); ++j){
                    if (lastPathNodes[j].postOrder == n.postOrder){
                        if (lastPathNodes[j].leftChild == -1){
                            // count because leftChild is the node that will be written to file now
                            lastPathNodes[j].leftChild = count;
                        } else {
                            lastPathNodes[j].rightChild = count;
                        }

                        lastPathNodes[i].id = count;
                        out.write(lastPathNodes[i]);
                        count++;
                        skip = true;
                    }
                }

                if (!skip){
                    lastPathNodes[i].id = count;
                    out.write(lastPathNodes[i]);
                    count++;

                    pathNode node = {n.postOrder, -1, lastPathNodes[i].id, -1};
                    lastPathNodes[i] = node;
                }
                skip = false;
            }
        }
    }
}

bool Triangulation::finished(){
    return path.size() - 1 == pathProgress;
}

Edge Triangulation::getNextEdge(){
    Node cur = getNode(path[pathProgress][0]);
    Node next = getNode(path[pathProgress + 1][0]);

    pathProgress++;
    return commonEdge(cur, next);
}

Edge Triangulation::commonEdge(Node a, Node b){
    Edge e;
    bool foundFirst = false;

    for (int i = 0; i < 3; i++){
        if (containsPoint(b.points, a.points[i])) {
            if (!foundFirst){
                e.a = a.points[i];
                foundFirst = true;
            } else {
                e.b = a.points[i];
            }
        }
    }

    //std::cout << e.a.x << " " << e.a.y << " " << e.b.x << " " << e.b.y << std::endl;

    return e;
}

bool Triangulation::containsPoint(Point_2 points[3], Point_2 p){
    return (p == points[0] || p == points[1] || p == points[2]);
}

std::vector<int> Triangulation::containsPoint(Point_2 points[3], std::vector<Point_2> targets){
    std::vector<int> result;
    int size = targets.size();
    for (int i = 0; i < size; ++i){
        Point_2 p = targets[i];
        if (p == points[0] || p == points[1] || p == points[2])
        {
            result.push_back(i);
        }
    }

    return result;
}

std::vector<Triangle> Triangulation::copyPolygon(){
    std::vector<Triangle> result;
    in.seek(0);
    Node n;
    while (in.can_read()){
        n = in.read();
        //std::cout << n.postOrder << std::endl;
        result.push_back(Triangle{n.points[0], n.points[1], n.points[2]});
    }

    return result;
}
