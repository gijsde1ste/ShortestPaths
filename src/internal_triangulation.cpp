#include "internal_triangulation.h"

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
    std::vector<int> step;
    step.push_back(n.postOrder);
    path.push_back(step);

    while (n.parent != -1){
        n = triangulation[n.parent];
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

Edge internal_triangulation::getNextEdge(){
    Node cur = triangulation[path[pathProgress][0]];
    Node next = triangulation[path[pathProgress + 1][0]];

    pathProgress++;
    return commonEdge(cur, next);
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
