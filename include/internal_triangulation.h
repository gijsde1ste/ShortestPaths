#ifndef INTERNAL_TRIANGULATION_H
#define INTERNAL_TRIANGULATION_H

#include <definition.h>
#include <Triangulation.h>
#include <time.h>

class internal_triangulation: public Triangulation
{
    public:
        void open(std::string file);
        Node getRoot();
        Edge getNextEdge();
        void createPath(Point_2 target);
        Point_2 getRandomPoint();
        std::vector<Triangle> copyPolygon();
        size_t size();

    protected:
        std::vector<Node> triangulation;
    private:

};

#endif // INTERNAL_TRIANGULATION_H
