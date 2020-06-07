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
    int triangleIndex = rand() % in.size();
    int vertexIndex = rand() % 3;

    return getNode(triangleIndex).points[vertexIndex];
}

void Triangulation::createPath(Point_2 target){
    path.clear();
    pathProgress = 0;

    in.seek(0);
    Node n;
    // find a triangle containing target by scanning triangulation
    while (in.can_read()){
        n = in.read();
        Point_2 *p = std::find(std::begin(n.points), std::end(n.points), target);
        if (p != std::end(n.points)) {
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
        Point_2 *p = std::find(std::begin(n.points), std::end(n.points), target);
        if (p != std::end(n.points)) {
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
        Point_2 *p = std::find(std::begin(b.points), std::end(b.points), a.points[i]);
        if (p != std::end(b.points)) {
            if (!foundFirst){
                e.a = *p;
                foundFirst = true;
            } else {
                e.b = *p;
            }
        }
    }

    //std::cout << e.a.x << " " << e.a.y << " " << e.b.x << " " << e.b.y << std::endl;

    return e;
}

std::vector<Triangle> Triangulation::copyPolygon(){
    std::vector<Triangle> result;
    in.seek(0);
    Node n;
    while (in.can_read()){
        Node n = in.read();
        result.push_back(Triangle{n.points[0], n.points[1], n.points[2]});
    }

    return result;
}
