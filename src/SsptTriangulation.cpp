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

        if (sspt->currentDtn->rightChild != -1){
            sspt->setCurrentDtn(&sspt->degree3Nodes[sspt->currentDtn->rightChild]);
        } else {
            // Hack a special kind of degreeThreeNode together, only used for the min/max/current deque which is all the same value for this leaf.
            DegreeThreeNode temp = {-1, -1, sspt->point2Zero, -1, -1, -1, 0, 0, sspt->point2Zero, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1};
            sspt->setCurrentDtn(&temp);
        }

        if (sspt->currentDtn->leftChild != -1){
            treeStack.push_back(std::tuple<int, Point_2, DegreeThreeNode*, int>{
                cur.id,
                sspt->getApex(),
                &sspt->degree3Nodes[sspt->currentDtn->leftChild],
                sspt->outputParent+1
            });
        } else {
            DegreeThreeNode temp = {-1, -1, sspt->point2Zero, -1, -1, -1, 0, 0, sspt->point2Zero, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1, sspt->currentDtn->deque + 1};
            treeStack.push_back(std::tuple<int, Point_2, DegreeThreeNode*, int>{
                cur.id,
                sspt->getApex(),
                &temp,
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
