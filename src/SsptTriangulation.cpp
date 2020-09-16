#include "SsptTriangulation.h"

bool SsptTriangulation::finished(){
    if (getNode(pathProgress).leftChild == -1 && treeStack.size() == 0){
        return true;
    }
    return false;
}

Edge SsptTriangulation::getNextEdge(SparseShortestPathTree * sspt){

    Node cur = getNode(pathProgress);
    std::cout << cur.id << " " << cur.leftChild << " " << cur.rightChild << std::endl;
    Node next;

    // Degree 3 node found, save it for later
    if (cur.rightChild != -1){
        next = getNode(cur.rightChild);
        DegreeThreeNode * prevDtn = sspt->currentDtn;

        if (sspt->currentDtn->rightChild != -1){
            sspt->splitFix();
            sspt->setCurrentDtn(&sspt->degree3Nodes[sspt->currentDtn->rightChild]);
        } else {
            // Hack a special kind of degreeThreeNode together, only used for the min/max/current deque which is all the same value for this leaf.
            DegreeThreeNode temp = {-1, -1, sspt->point2Zero, -1, -1, -1, 0, 0, sspt->point2Zero, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1};
            sspt->splitFix();
            sspt->setCurrentDtn(&temp);
        }

        if (prevDtn->leftChild != -1){
            treeStack.push_back(std::tuple<int, Point_2, DegreeThreeNode*, int>{
                cur.id,
                sspt->getApex(),
                &sspt->degree3Nodes[prevDtn->leftChild],
                sspt->outputParent+1
            });
        } else {
            DegreeThreeNode temp = {-1, -1, sspt->point2Zero, -1, -1, -1, 0, 0, sspt->point2Zero, prevDtn->deque, prevDtn->deque, prevDtn->deque};
            tempNodes.push_back(temp);
            std::cout << "saved deque: " << sspt->currentDtn->deque << std::endl;
            treeStack.push_back(std::tuple<int, Point_2, DegreeThreeNode*, int>{
                cur.id,
                sspt->getApex(),
                &tempNodes.back(),
                sspt->outputParent+1
            });
        }

    } else if (cur.leftChild != -1){
        next = getNode(cur.leftChild);
    } else {
        std::tuple<int, Point_2, DegreeThreeNode*, int> node = treeStack.back();
        treeStack.pop_back();

        cur = getNode(std::get<0>(node));
        next = getNode(cur.leftChild);
        sspt->setApex(std::get<1>(node));
        sspt->setCurrentDtn(std::get<2>(node));
        sspt->outputParent = std::get<3>(node);
    }

    pathProgress = next.id;
    return commonEdge(cur, next);
}

void SsptTriangulation::createPath(std::vector<Point_2> targets){

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
