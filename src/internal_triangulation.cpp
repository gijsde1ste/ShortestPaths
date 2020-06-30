#include "internal_triangulation.h"

internal_triangulation::internal_triangulation()
{
    path = std::vector<int>();
    pathProgress = 0;
    srand(time(NULL));
}

void  internal_triangulation::open(std::string file)
{
    in.open(file);

    while (in.can_read()){
        triangulation.push_back(in.read());
    }

    in.close();
}

Point_2 internal_triangulation::getRandomPoint()
{
    // root of tree
    Node source = getRoot();
    Point_2 result;
    while (true){
        int triangleIndex = rand() % triangulation.size();
        int vertexIndex = rand() % 3;
        result = triangulation[triangleIndex].points[vertexIndex];

        // if the random point we found is not part of source triangle
        if (!containsPoint(source.points, result)) break;
    }

    return result;
}

Node internal_triangulation::getRoot(){
    return triangulation[triangulation.size() - 1];
}

void internal_triangulation::createPath(Point_2 target){
    path.clear();
    pathProgress = 0;

    // find a triangle containing target by scanning triangulation
    Node n;
    for (auto i = triangulation.begin(); i != triangulation.end(); i++)
    {
        if (containsPoint((*i).points, target)) {
            // a target triangle has been found
            n = *i;
            break;
        }
    }

    // construct the path from n to the source (root)
    // its possible we find a triangle closer to the root that also contains the target point
    path.push_back(n.postOrder);
    while (n.parent != -1){
        n = triangulation[n.parent];
        // Check if parent contains target point, if so this is a shorter path to the root
        if (containsPoint(n.points, target)) {
            path.clear();
        }
        path.push_back(n.postOrder);
    }

    // Orient away from source
    std::reverse(path.begin(), path.end());
}

bool internal_triangulation::finished(){
    return path.size() - 1 == pathProgress;
}

Edge internal_triangulation::getNextEdge(){
    Node cur = triangulation[path[pathProgress]];
    Node next = triangulation[path[pathProgress + 1]];

    pathProgress++;
    return commonEdge(cur, next);
}

Edge internal_triangulation::commonEdge(Node a, Node b){
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

bool internal_triangulation::containsPoint(Point_2 points[3], Point_2 p){
    return (p == points[0] || p == points[1] || p == points[2]);
}

std::vector<Triangle> internal_triangulation::copyPolygon(){
    std::vector<Triangle> result;
    for (auto i = triangulation.begin(); i != triangulation.end(); i++)
    {
        //std::cout << n.postOrder << std::endl;
        result.push_back(Triangle{(*i).points[0], (*i).points[1], (*i).points[2]});
    }

    return result;
}

size_t internal_triangulation::size(){
    return triangulation.size();
}
