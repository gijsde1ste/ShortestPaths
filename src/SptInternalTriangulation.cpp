#include "SptInternalTriangulation.h"

Edge SptInternalTriangulation::getNextEdge(){

    Node cur = triangulation[pathProgress];
    Node next;
    if (cur.rightChild != -1 && triangulation[cur.rightChild].preOrder != 1){
        next = triangulation[cur.rightChild];
        // Abuse pre order integer as visited boolean
        triangulation[cur.rightChild].preOrder = 1;
    } else if (cur.leftChild != -1 && triangulation[cur.leftChild].preOrder != 1){
        next = triangulation[cur.leftChild];
        // Abuse pre order integer as visited boolean
        triangulation[cur.leftChild].preOrder = 1;
    } else {
        // No more available children, backtrack till we find a split where we have not yet visited left and right childs
        pathProgress = cur.parent;
        std::cout << "backTracking" << std::endl;
        return edgeZero;
    }

    pathProgress = next.postOrder;
    std::cout << "current node " << cur.postOrder << " next node " << next.postOrder << std::endl;
    return commonEdge(cur, next);
}

bool SptInternalTriangulation::finished(){
    return (triangulation[0].preOrder == 1);
}
