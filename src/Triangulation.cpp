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

std::vector<DegreeThreeNode> Triangulation::openUserData()
{
    in.read_user_data(degree3Nodes);
    return degree3Nodes;
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

Node Triangulation::getNode(int index, bool inOrOut)
{
    if (!inOrOut){
        in.seek(index);
        return in.read();
    } else {
        out.seek(index);
        return out.read();
    }
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
    std::vector<Node> lastNodes(targets.size());
    // find triangles containing targets by scanning triangulation
    while (in.can_read()){

        n = in.read();

        std::vector<int> foundTargets = containsPoint(n.points, targets);
        // The logic here is, if node n contains one of our target points we store it
        // If the postOrder number is bigger for the same target it is closer to the source point
        // Since input is sorted in postOrder always override previous value
        for (auto it = foundTargets.begin(); it != foundTargets.end(); ++it){
            lastNodes[*it] = {{n.points[0], n.points[1], n.points[2]}, -1, -1, -1, -1, n.postOrder, -1};
        }
    }

    std::sort(lastNodes.begin(), lastNodes.end(), [](Node a, Node b){
        return a.postOrder < b.postOrder;
    });


    in.seek(0);
    int count = 0;
    bool foundChild = false;
    Point_2 lastPoints[3];

    out.open("leafToRoot.tpie", tpie::access_read_write, 2048);
    out.truncate(0);
    std::vector<DegreeThreeNode> metaData;

    while (in.can_read()){
        n = in.read();

        Node nodeOut = {{{0, 0}, {0, 0}, {0,0}}, -1, -1, -1, -1, -1, -1};

        for (int i = 0; i < targets.size(); ++i){
            if (n.postOrder == lastNodes[i].postOrder){
                if (!foundChild){
                    foundChild = true;
                    // fill lastpoints so we can use them properly if we find a deg-3 node
                    lastPoints[0] = lastNodes[i].points[0];
                    lastPoints[1] = lastNodes[i].points[1];
                    lastPoints[2] = lastNodes[i].points[2];

                    nodeOut.id = count;
                    std::copy(std::begin(n.points), std::end(n.points), std::begin(nodeOut.points));
                    nodeOut.leftChild = lastNodes[i].leftChild;
                    nodeOut.postOrder = n.postOrder;

                    // create new node, only need leftChild and postOrder number to function
                    // points are initialised to this (previous) node so we can easily
                    // find split vertex if its parent is degree 3 node
                    lastNodes[i] = {{n.points[0], n.points[1], n.points[2]}, -1, -1, nodeOut.id, -1, n.parent, -1};
                } else if(lastNodes[i].leftChild != -1){

                    // Degree 3 node found
                    nodeOut.rightChild = lastNodes[i].leftChild;

                    // Find split vertex
                    Point_2 splitVertex;
                    for (int x = 0; x < 3; x++){
                        if (containsPoint(lastNodes[i].points, lastPoints[x])){
                            splitVertex = lastPoints[x];
                            break;
                        }
                    }

                    metaData.push_back({metaData.size(), n.postOrder, splitVertex, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1});
                }
            }
        }

        // Node is used, so write it
        if (foundChild) {
            foundChild = false;
            out.write(nodeOut);
            count++;
        }
    }

    in.close();

    // scan the previously generated file backwards to get the metaData tree in correct order
    std::stack<std::pair<int, int>> history; // max k degree 3 nodes, easily fits in main memory
    int index = metaData.size() - 1;
    int current = -1;

    out.seek(0, out.end);
    n = out.read_back();

    while (index >= 0){
        if (n.rightChild != -1){

            history.push({index, n.leftChild});

            if (current != -1){
                if (metaData[current].rightChild == -1){
                    metaData[current].rightChild = index;
                    metaData[index].parentNode = current;
                } else {
                    metaData[current].leftChild = index;
                    metaData[index].parentNode = current;
                }
            }

            current = index;
            index--;
            n = getNode(n.rightChild, true);
            continue;
        } else {
            if (n.leftChild != -1){
                n = getNode(n.leftChild, true);
            } else {
                std::pair<int, int> last = history.top();
                history.pop();

                current = last.first;
                n = getNode(last.second, true);
            }
        }
    }

    SplitVertices = metaData;

    out.seek(0, out.end);
    out.write_user_data(metaData);
    out.close();
}

bool Triangulation::finished(bool shortestPathTree){
    if (shortestPathTree){
        if (getNode(pathProgress).leftChild == -1 && treeStack.size() == 0){
            return true;
        }
        return false;
    } else {
        return path.size() - 1 == pathProgress;
    }
}

Edge Triangulation::getNextEdge(bool shortestPathTree, SparseShortestPathTree * sspt){
    if (shortestPathTree){
        Node cur = getNode(pathProgress);
        std::cout << cur.id << " " << cur.leftChild << " " << cur.rightChild << std::endl;
        Node next;

        // Degree 3 node found, save it for later
        if (cur.rightChild != -1){
            next = getNode(cur.rightChild);

            if (sspt->currentDtn->rightChild != -1){
                sspt->setCurrentDtn(&sspt->degree3Nodes[sspt->currentDtn->rightChild]);
            } else {
                // Hack a special kind of degreeThreeNode together, only used for the min/max/current deque which is all the same value for this leaf.
                DegreeThreeNode temp = {-1, -1, sspt->point2Zero, -1, -1, -1, 0, 0, sspt->point2Zero, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1};
                sspt->setCurrentDtn(&temp);
            }

            if (sspt->currentDtn->leftChild != -1){
                treeStack.push_back(std::tuple<int, Point_2, DegreeThreeNode*>{cur.id, sspt->getApex(), &sspt->degree3Nodes[sspt->currentDtn->leftChild]});
            } else {
                DegreeThreeNode temp = {-1, -1, sspt->point2Zero, -1, -1, -1, 0, 0, sspt->point2Zero, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1};
                treeStack.push_back(std::tuple<int, Point_2, DegreeThreeNode*>{cur.id, sspt->getApex(), &temp});
            }

        } else if (cur.leftChild != -1){
            next = getNode(cur.leftChild);
        } else {
            std::tuple<int, Point_2, DegreeThreeNode*> node = treeStack.back();
            treeStack.pop_back();

            cur = getNode(std::get<0>(node));
            next = getNode(cur.leftChild);
            sspt->setApex(std::get<1>(node));
            sspt->setCurrentDtn(std::get<2>(node));
        }

        pathProgress = next.id;
        return commonEdge(cur, next);
    } else {
        Node cur = getNode(path[pathProgress][0]);
        Node next = getNode(path[pathProgress + 1][0]);

        pathProgress++;
        return commonEdge(cur, next);
    }
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

void Triangulation::setPathProgress(int progress){
    pathProgress = progress;
}
