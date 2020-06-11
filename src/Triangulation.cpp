#include "Triangulation.h"

Triangulation::Triangulation()
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
    path.push_back(n.postOrder);
    while (n.parent != -1){
        n = getNode(n.parent);
        // Check if parent contains target point, if so this is a shorter path to the root
        if (containsPoint(n.points, target)) {
            path.clear();
        }
        path.push_back(n.postOrder);
    }

    // Orient away from source
    std::reverse(path.begin(), path.end());
}

bool Triangulation::finished(){
    return path.size() - 1 == pathProgress;
}

Edge Triangulation::getNextEdge(){
    Node cur = getNode(path[pathProgress]);
    Node next = getNode(path[pathProgress + 1]);

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

std::vector<Triangle> Triangulation::copyPolygon(){
    std::vector<Triangle> result;
    in.seek(0);
    Node n;
    while (in.can_read()){
        Node n = in.read();
        //std::cout << n.postOrder << std::endl;
        result.push_back(Triangle{n.points[0], n.points[1], n.points[2]});
    }

    return result;
}
