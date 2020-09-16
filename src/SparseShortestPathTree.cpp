#include "SparseShortestPathTree.h"

SparseShortestPathTree::SparseShortestPathTree(std::vector<DegreeThreeNode> degreeThreeNodes)
{
    outputCounter = 0;
    outputParent = -1;
    out.open("result.tpie", tpie::access_write);
    out.truncate(0);

    point2Zero.x = 0; point2Zero.y = 0;
    degree3Nodes = degreeThreeNodes;

    cuspCount = degree3Nodes.size() + 1;
    // Create deques between the degree 3 nodes
    for (int i = 0; i < cuspCount; i++){
        cusps.push_back(new tpie::deque<Point_2>());
    }

    minCusp = 0;
    maxCusp = cuspCount - 1;
    cuspChanged = false;

    // Assign number of left/right childs to degree3Nodes to known which deques should be used at split
    currentDtn = &degree3Nodes.back();
    assignNChilds(currentDtn);
    assignDeques(currentDtn, 0, cusps.size() - 1);
    assignDegreeThreeIndex(currentDtn);
    currentCusp = currentDtn->nLeftChilds;
    cusp = cusps[currentCusp];

    //geodesic = new tpie::stack<Point_2>();

    /*for (auto i = degree3Nodes.begin(); i != degree3Nodes.end(); ++i){
        std::cout << i->id << " " << i->leftChild << " " << i->rightChild << " " << i->parentNode << " "  << i->vertex.x << " " << i->vertex.y
        << " --- " << i->nLeftChilds << " " << i->nRightChilds << " --- " << i->minDeque << " " << i->deque << " " << i->maxDeque << std::endl;
    }

    for (auto i = degree3NodesIndex.begin(); i != degree3NodesIndex.end(); ++i){
        std::cout << *i << std::endl;
    }*/

}

int SparseShortestPathTree::assignNChilds(DegreeThreeNode * dtn){
    if (dtn->leftChild != -1){
        dtn->nLeftChilds = assignNChilds(&degree3Nodes[dtn->leftChild]);
    } else {
        dtn->nLeftChilds = 0;
    }

    if (dtn->rightChild != -1){
        dtn->nRightChilds = assignNChilds(&degree3Nodes[dtn->rightChild]);
    } else {
        dtn->nRightChilds = 0;
    }

    dtn->parent = point2Zero;
    return dtn->nLeftChilds + dtn->nRightChilds + 1;
}

void SparseShortestPathTree::assignDeques(DegreeThreeNode * dtn, int min, int max){
    dtn->minDeque = min;
    dtn->maxDeque = max;
    dtn->deque = min + dtn->nLeftChilds;

    if (dtn->leftChild != -1){
        assignDeques(&degree3Nodes[dtn->leftChild], min, dtn->deque);
    }

    if (dtn->rightChild != -1){
        assignDeques(&degree3Nodes[dtn->rightChild], dtn->deque + 1, max);
    }
}

void SparseShortestPathTree::assignDegreeThreeIndex(DegreeThreeNode * dtn){
    if (dtn->leftChild != -1){
        assignDegreeThreeIndex(&degree3Nodes[dtn->leftChild]);
    }

    degree3NodesIndex.push_back(dtn->id);
    dtn->indexId = degree3NodesIndex.size() - 1;

    if (dtn->rightChild != -1){
        assignDegreeThreeIndex(&degree3Nodes[dtn->rightChild]);
    }
}

void SparseShortestPathTree::extendStart(Node source, Edge e){
    // source = apex should be the point of source triangle that is not on the first added edge
    for (int i = 0; i < 3; i++){
        apex = source.points[i];
        if (!(apex == e.a) && !(apex == e.b)) break;
    }

    currentDtn->parent = apex;
    Point_2 chosenVertex;
    if (rightTurn(e.a, apex, e.b))
    {
        chosenVertex = e.b;
    } else {
        chosenVertex = e.a;
    }

    if (leftTurn(apex, chosenVertex, currentDtn->vertex)){
        currentDtn->parent = chosenVertex;

        DegreeThreeNode * dtn;
        bool found = false;
        for (int i = currentDtn->indexId + 1; i < currentDtn->maxDeque; i++){
            dtn = &degree3Nodes[degree3NodesIndex[i]];
            if (rightTurn(apex, chosenVertex, dtn->vertex)){
                dtn->parent = apex;
                found = true;
                break;
            }
        }

        if (!found){
            cusps[currentDtn->maxDeque]->pushFront(apex);
        }
    } else {
        DegreeThreeNode * dtn;
        bool found = false;
        for (int i = currentDtn->indexId - 1; i >= currentDtn->minDeque; i--){
            dtn = &degree3Nodes[degree3NodesIndex[i]];
            if (leftTurn(apex, chosenVertex, dtn->vertex)){
                dtn->parent = chosenVertex;
                found = true;
                break;
            }
        }

        if (!found){
            std::cout << " not here " << std::endl;
            cusps[currentDtn->minDeque]->pushBack(chosenVertex);
        }
    }
    //printFunnel();
    extend({e.a, e.b});
    //printFunnel();
}

void SparseShortestPathTree::extend(Edge e){
    Point_2 a = e.a;
    Point_2 b = e.b;

    std::cout << "Adding " << a.x << " " << a.y << " and " << b.x << " " << b.y << std::endl;

    // one of the points should already be part of the cusp
    if (a == peekFront()){
        std::cout << "1" << std::endl;
        extend(b, false);
    } else if (a == peekBack()){
        std::cout << "2" << std::endl;
        extend(b, true);
    } else if (b == peekFront()){
        std::cout << "3" << std::endl;
        extend(a, false);
    } else if (b == peekBack()){
        std::cout << "4" << std::endl;
        extend(a, true);
    } else {
        std::cout << "Looking for: " << e.a.x << " " << e.a.y << " and " << e.b.x << " " << e.b.y << std::endl;
        std::cout << "Found front: " << peekFront().x <<  " " << peekFront().y << " Back: " << peekBack().x << " " << peekBack().y << std::endl;
        // This should only happen if degree3Node had changed and no points have been added to those queues
        // In that case we should start again with the correct points
        // One of the points should be at the end of one of the deques, so locate that
        /*cusp = cusps[currentDtn->deque];
        for (auto i = cusps.begin(); i != cusps.end(); ++i){
            if ((*i)->peekFront() == a){
                apex = a;
                cusp->pushFront(a);
                cusp->pushBack(b);
                break;
            } else if ((*i)->peekBack() == a){
                apex = a;
                cusp->pushBack(a);
                cusp->pushFront(b);
                break;
            } else if ((*i)->peekFront() == b){
                apex = b;
                cusp->pushFront(b);
                cusp->pushBack(a);
                break;
            } else if ((*i)->peekBack() == b){
                apex = b;
                cusp->pushBack(b);
                cusp->pushFront(a);
                break;
            }
        }*/
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
            if (!cuspChanged) currentCusp = dtn->deque;
            cuspChanged = true;
        }
    } else {
        if (leftTurn(peekBack(), p, dtn->vertex)){
            dtn->parent = p;
            if (!cuspChanged) currentCusp = dtn->deque;
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

    if (front){
        while(true){
            a = popFront();
            b = peekFront();

            if (b == point2Zero){
                // Rare case if a new point is added that is only tangent with the very last point
                undoPop(a);
                apex = peekFront();
                pushFront(p);
                break;
            }

            if (a == apex) passedApex = true;

            if (!passedApex){
                if (leftTurn(p, a, b)){
                    std::cout << "leftTurn with apex: " << apex.x << " " << apex.y << " and a: " << a.x << " " << a.y << std::endl;
                    undoPop(a);
                    pushFront(p);
                    break;
                } else {
                    std::cout << "Nothing happened" << std::endl;
                }
            } else {
                if (rightTurn(p, a, b)){
                    std::cout << "Rightturn" << std::endl;
                    undoPop(a);
                    apex = peekFront();
                    pushFront(p);
                    break;
                } else {
                    std::cout << "Output added" << std::endl;
                    /*ssptResultPoint srp = {a, outputParent};
                    out.write(srp);
                    outputParent = outputCounter;
                    outputCounter++;*/

                    //geodesic->push(a);
                }
            }
        }
    } else {
        while(true){
            a = popBack();
            b = peekBack();

            if (b == point2Zero){
                // Rare case if a new point is added that is only tangent with the very last point
                undoPop(a);
                apex = peekBack();
                pushBack(p);
                break;
            }

            if (a == apex) passedApex = true;

            if (!passedApex){
                if (rightTurn(p, a, b)){
                    std::cout << "Rightturn" << std::endl;
                    undoPop(a);
                    pushBack(p);
                    break;
                } else {
                    std::cout << "Nothing happened" << std::endl;
                }
            } else {
                if (leftTurn(p, a, b)){
                    std::cout << "Leftturn" << std::endl;
                    undoPop(a);
                    apex = peekBack();
                    pushBack(p);
                    break;
                } else {
                    std::cout << "Output added" << std::endl;
                    /*ssptResultPoint srp = {a, outputParent};
                    out.write(srp);
                    outputParent = outputCounter;
                    outputCounter++;*/
                    //geodesic->push(a);
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

            //if (apexFound) geodesic->push(p);
        }
    }
    else
    {
        extend(target, false);
        Point_2 p;
        while (cusp->size()){
            p = cusp->popFront();

            if (p == apex) apexFound = true;

            //if (apexFound) geodesic->push(p);
        }
    }
}

void SparseShortestPathTree::splitFix(){
    if (currentDtn->rightChild != -1 && degree3Nodes[currentDtn->rightChild].parent == point2Zero){
        degree3Nodes[currentDtn->rightChild].parent = currentDtn->parent;
    } else {
        cusps[currentDtn->indexId + 1]->pushBack(currentDtn->parent);
    }

    if (currentDtn->leftChild != -1 && degree3Nodes[currentDtn->leftChild].parent == point2Zero){
        degree3Nodes[currentDtn->leftChild].parent = currentDtn->parent;
    } else {
        cusps[currentDtn->indexId]->pushBack(currentDtn->parent);
    }
}

Point_2 SparseShortestPathTree::peekFront()
{
    for (int i = maxCusp; i >= minCusp; i--){
        if (i != maxCusp){
            if (!(degree3Nodes[degree3NodesIndex[i]].parent == point2Zero)){
                return degree3Nodes[degree3NodesIndex[i]].parent;
            }
        }

        if (!cusps[i]->empty()){
            return cusps[i]->peekBack();
        }
    }

    return point2Zero;
}

Point_2 SparseShortestPathTree::peekBack()
{
    for (int i = minCusp; i <= maxCusp; i++){
        if (!cusps[i]->empty()){
            return cusps[i]->peekFront();
        }
        if (i != maxCusp){
            if (!(degree3Nodes[degree3NodesIndex[i]].parent == point2Zero)){
                return degree3Nodes[degree3NodesIndex[i]].parent;
            }
        }
    }

    return point2Zero;
}

Point_2 SparseShortestPathTree::popFront()
{
    for (int i = maxCusp; i >= minCusp; i--){
        if (i != maxCusp){
            Point_2 p = degree3Nodes[degree3NodesIndex[i]].parent;
            if (!(p == point2Zero)){
                degree3Nodes[degree3NodesIndex[i]].parent = point2Zero;
                lastPop.index = i;
                lastPop.splitVertex = true;
                return p;
            }
        }

        if (!cusps[i]->empty()){
            lastPop.front = false;
            lastPop.index = i;
            lastPop.splitVertex = false;
            return cusps[i]->popBack();
        }

    }

    return point2Zero;
}

Point_2 SparseShortestPathTree::popBack()
{
    for (int i = minCusp; i <= maxCusp; i++){
        if (!cusps[i]->empty()){
            lastPop.front = true;
            lastPop.index = i;
            lastPop.splitVertex = false;
            return cusps[i]->popFront();
        }
        if (i != maxCusp){
            Point_2 p = degree3Nodes[degree3NodesIndex[i]].parent;
            if (!(p == point2Zero)){
                degree3Nodes[degree3NodesIndex[i]].parent = point2Zero;
                lastPop.index = i;
                lastPop.splitVertex = true;
                return p;
            }
        }
    }

    return point2Zero;
}


void SparseShortestPathTree::pushFront(Point_2 p){
    Point_2 prev = peekFront();

    // pushing always happens just after a undoPop, so lastPop.index has the cusp/splitvertex that has been added too last
    bool found = false;
    if (minCusp != maxCusp){
        for (int i = lastPop.index; i < maxCusp; i++){
            if (rightTurn(prev, p, degree3Nodes[degree3NodesIndex[i]].vertex)){
                // Tangent splitvertex has been found, if its the first one we need to write that vertex to it's previous cusp
                if (!(degree3Nodes[degree3NodesIndex[i]].parent == point2Zero)){
                    cusps[i]->pushBack(degree3Nodes[degree3NodesIndex[i]].parent);
                }
                degree3Nodes[degree3NodesIndex[i]].parent = p;
                found = true;
                break;
            }
        }
    }

    // If no tangent splitvertex has been found, add it to the left most deque
    if (!found){
        cusps[maxCusp]->pushBack(p);
    }
}


void SparseShortestPathTree::pushBack(Point_2 p){
    Point_2 prev = peekBack();
    // pushing always happens just after a undoPop, so lastPop.index has the cusp/splitvertex that has been added too last
    bool found = false;
    if (minCusp != maxCusp){
        for (int i = lastPop.index; i >= minCusp; i--){
            if (leftTurn(prev, p, degree3Nodes[degree3NodesIndex[i]].vertex)){
                // Tangent splitvertex has been found, if its the first one we need to write that vertex to it's previous cusp
                if (!(degree3Nodes[degree3NodesIndex[i]].parent == point2Zero)){
                    cusps[i+1]->pushFront(degree3Nodes[degree3NodesIndex[i]].parent);
                }

                degree3Nodes[degree3NodesIndex[i]].parent = p;
                found = true;
                break;
            }
        }
    }

    // If no tangent splitvertex has been found, add it to the left most deque
    if (!found){
        cusps[minCusp]->pushFront(p);
    }
}


void SparseShortestPathTree::undoPop(Point_2 p){
    if (lastPop.splitVertex){
        degree3Nodes[degree3NodesIndex[lastPop.index]].parent = p;
    } else {
        if (lastPop.front){
            cusps[lastPop.index]->pushFront(p);
        } else {
            cusps[lastPop.index]->pushBack(p);
        }
    }
}


int SparseShortestPathTree::getCuspCount()
{
    return cuspCount;
}


void SparseShortestPathTree::setCurrentDtn(DegreeThreeNode * dtn){
    currentDtn = dtn;

    minCusp = dtn->minDeque;
    maxCusp = dtn->maxDeque;
}


void SparseShortestPathTree::printFunnel(){
    std::cout << "Apex: " << apex.x << " " << apex.y << std::endl;
    Point_2 p;
    std::cout << minCusp << " " << maxCusp << std::endl;
    for (int i = minCusp; i <= maxCusp; i++){

        std::cout << "Deque: " << i << std::endl;
        while (!cusps[i]->empty()){
            p = cusps[i]->popFront();
            std::cout << p.x << " " << p.y << std::endl;
        }

        if (i != maxCusp){
            p = degree3Nodes[degree3NodesIndex[i]].parent;
            std::cout << "DTN: " << i << " parent vertex: " << p.x << " " << p.y << std::endl;
        }
    }
}
