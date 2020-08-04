#include "SparseShortestPathTree.h"

SparseShortestPathTree::SparseShortestPathTree(std::vector<DegreeThreeNode> degreeThreeNodes)
{
    point2Zero.x = 0; point2Zero.y = 0;
    degree3Nodes = degreeThreeNodes;
    cuspCount = degree3Nodes.size();

    // Create deques between the degree 3 nodes
    for (int i = 0; i < cuspCount; i++){
        cusps.push_back(new tpie::deque<Point_2>());
    }

    // Create deques for every leaf degree 3 node
    for (auto i = degree3Nodes.begin(); i != degree3Nodes.end(); ++i){
        if (i->leftChild == -1){
            cusps.push_back(new tpie::deque<Point_2>());
            cuspCount++;
        }

        if (i->rightChild == -1){
            cusps.push_back(new tpie::deque<Point_2>());
            cuspCount++;
        }
    }

    minCusp = 0;
    maxCusp = cusps.size() - 1;
    cuspChanged = false;

    // Assign number of left/right childs to degree3Nodes to known which deques should be used at split
    currentDtn = &degree3Nodes.back();
    assignNChilds(currentDtn);
    currentCusp = currentDtn->nLeftChilds;
    cusp = cusps[currentCusp];

    geodesic = new tpie::stack<Point_2>();
    /*
    cuspCount = 0;
    cusps.push_back(new tpie::deque<Point_2>());
    cusp = cusps[cuspCount];

    stackCount = 0;
    stacks.push_back(new tpie::stack<Point_2>());
    geodesic = stacks[stackCount];
    */
}

int SparseShortestPathTree::assignNChilds(DegreeThreeNode * dtn){
    if (dtn->leftChild != -1){
        dtn->nLeftChilds = assignNChilds(&degree3Nodes[dtn->leftChild]);
    } else {
        dtn->nLeftChilds = 1;
    }

    if (dtn->rightChild != -1){
        dtn->nRightChilds = assignNChilds(&degree3Nodes[dtn->rightChild]);
    } else {
        dtn->nRightChilds = 1;
    }

    return dtn->nLeftChilds + dtn->nRightChilds + 1;
}

void SparseShortestPathTree::extendStart(Node source, Edge e){
    // source = apex should be the point of source triangle that is not on the first added edge
    for (int i = 0; i < 3; i++){
        apex = source.points[i];
        if (!(apex == e.a) && !(apex == e.b)) break;
    }

    cusp->pushFront(apex);

    if (rightTurn(e.a, apex, e.b))
    {
        cusp->pushFront(e.a);
        cusp->pushBack(e.b);
    } else {
        cusp->pushFront(e.b);
        cusp->pushBack(e.a);
    }

    // Assign parents of all fingers (degree 3 nodes) to source
    for (auto i = degree3Nodes.begin(); i != degree3Nodes.end(); ++i){
        if (rightTurn(apex, cusp->peekFront(), i->vertex)){
            i->parent = cusp->peekFront();
        } else if (leftTurn(apex, cusp->peekBack(), i->vertex)){
            i->parent = cusp->peekBack();
        } else {
            i->parent = apex;
        }
    }
}

void SparseShortestPathTree::extend(Edge e){
    Point_2 a = e.a;
    Point_2 b = e.b;

    // one of the points should already be part of the cusp

    if (a == peekFront()){
        extend(b, false);
    } else if (a == peekBack()){
        extend(b, true);
    } else if (b == peekFront()){
        extend(a, false);
    } else if (b == peekBack()){
        extend(a, true);
    }
}

int SparseShortestPathTree::getDeque(DegreeThreeNode * dtn){

    DegreeThreeNode * parent;
    int dequeIndex = dtn->nLeftChilds;
    while (dtn->parentNode != -1){
        parent = &degree3Nodes[dtn->parentNode];

        if (dtn->id == parent->rightChild){
            dequeIndex += parent->nLeftChilds;
        }

        dtn = parent;
    }

    return dequeIndex;
}

void SparseShortestPathTree::assignFingerParents(DegreeThreeNode * dtn, Point_2 p, bool front){
    if (front){
        if (rightTurn(peekFront(), p, dtn->vertex)){
            dtn->parent = p;
            if (!cuspChanged) currentCusp = getDeque(dtn);
            cuspChanged = true;
        }
    } else {
        if (leftTurn(peekBack(), p, dtn->vertex)){
            dtn->parent = p;
            if (!cuspChanged) currentCusp = getDeque(dtn);
            cuspChanged = true;
        }
    }

    if (dtn->leftChild != -1){
        assignFingerParents(&degree3Nodes[dtn->leftChild], p, front);
    }

    if (dtn->rightChild != -1){
        assignFingerParents(&degree3Nodes[dtn->rightChild], p, front);
    }
}

void SparseShortestPathTree::extend(Point_2 p, bool front) {

    Point_2 a, b;
    bool passedApex = false;

    cuspChanged = false;
    // Handle the parents of fingers
    if (front){
        if (rightTurn(peekFront(), p, currentDtn->vertex)){
            currentDtn->parent = p;
            currentCusp = getDeque(currentDtn);
            cuspChanged = true;
        }

        if (currentDtn->leftChild != -1){
            assignFingerParents(&degree3Nodes[currentDtn->leftChild], p, front);
        }
    } else {
        if (leftTurn(cusp->peekBack(), p, currentDtn->vertex)){
            currentDtn->parent = p;
            currentCusp = getDeque(currentDtn);
            cuspChanged = true;
        }

        if (currentDtn->rightChild != -1){
            assignFingerParents(&degree3Nodes[currentDtn->rightChild], p, front);
        }
    }
    // Set cusp to add vertices the correct cusp with regards to changing fingers
    cusp = cusps[currentCusp];


    if (front){
        while(true){
            a = popFront();
            b = peekFront();

            if (b == point2Zero){
                // Rare case if a new point is added that is only tangent with the very last point
                cusp->pushFront(a);
                apex = cusp->peekFront();
                cusp->pushFront(p);
                break;
            }

            if (a == apex) passedApex = true;

            if (!passedApex){
                if (leftTurn(p, a, b)){
                    cusp->pushFront(a);
                    cusp->pushFront(p);
                    break;
                }
            } else {
                if (rightTurn(p, a, b)){
                    cusp->pushFront(a);
                    apex = cusp->peekFront();
                    cusp->pushFront(p);
                    break;
                } else {
                    geodesic->push(a);
                }
            }
        }
    } else {
        while(true){
            a = popBack();
            b = peekBack();

            if (b == point2Zero){
                // Rare case if a new point is added that is only tangent with the very last point
                cusp->pushBack(a);
                apex = peekBack();
                cusp->pushBack(p);
                break;
            }

            if (a == apex) passedApex = true;

            if (!passedApex){
                if (rightTurn(p, a, b)){
                    cusp->pushBack(a);
                    cusp->pushBack(p);
                    break;
                }
            } else {
                if (leftTurn(p, a, b)){
                    cusp->pushBack(a);
                    apex = cusp->peekBack();
                    cusp->pushBack(p);
                    break;
                } else {
                    geodesic->push(a);
                }
            }
        }
    }


}

void SparseShortestPathTree::extendFinalStep(Point_2 target){
    // This is a little bit backwards because points have to be added in the right order
    // So first skip through the "wrong" side till we find the apex then add the rest
    bool apexFound = false;
    if (target.distance(cusp->peekFront()) < target.distance(cusp->peekBack()))
    {
        extend(target, true);
        Point_2 p;
        while (cusp->size()){
            p = cusp->popBack();

            if (p == apex) apexFound = true;

            if (apexFound) geodesic->push(p);
        }
    }
    else
    {
        extend(target, false);
        Point_2 p;
        while (cusp->size()){
            p = cusp->popFront();

            if (p == apex) apexFound = true;

            if (apexFound) geodesic->push(p);
        }
    }
}

Point_2 SparseShortestPathTree::peekFront()
{
    for (int i = minCusp; i <= maxCusp; i++){
        if (!cusps[i]->empty()){
            return cusps[i]->peekFront();
        }
    }

    return point2Zero;
}

Point_2 SparseShortestPathTree::peekBack()
{
    for (int i = maxCusp; i >= minCusp; i--){
        if (!cusps[i]->empty()){
            return cusps[i]->peekBack();
        }
    }

    return point2Zero;
}

Point_2 SparseShortestPathTree::popFront()
{
    for (int i = minCusp; i <= maxCusp; i++){
        if (!cusps[i]->empty()){
            return cusps[i]->popFront();
        }
    }

    return point2Zero;
}

Point_2 SparseShortestPathTree::popBack()
{
    for (int i = maxCusp; i >= minCusp; i--){
        if (!cusps[i]->empty()){
            return cusps[i]->popBack();
        }
    }

    return point2Zero;
}

int SparseShortestPathTree::getCuspCount()
{
    return cuspCount;
}

int SparseShortestPathTree::getStackCount()
{
    return stackCount;
}
