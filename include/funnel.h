#ifndef FUNNEL_H
#define FUNNEL_H

#include <base_funnel.h>
#include <tpie/stack.h>
#include <deque.h>
#include <Triangulation.h>


class funnel: public base_funnel
{
    public:
        funnel(double blockFactor = 1.0) {
            geodesic = tpie::stack<Point_2>(blockFactor);
            cusp = tpie::deque<Point_2>(blockFactor);
        }

        inline void extendStart(Node source, Edge e){
            // source = apex should be the point of source triangle that is not on the first added edge
            for (int i = 0; i < 3; i++){
                apex = source.points[i];
                if (!(apex == e.a) && !(apex == e.b)) break;
            }
            cusp.pushFront(apex);

            if (rightTurn(e.a, apex, e.b))
            {
                cusp.pushFront(e.a);
                cusp.pushBack(e.b);
            } else {
                cusp.pushFront(e.b);
                cusp.pushBack(e.a);
            }
        }

        inline void reset(){
            geodesic = tpie::stack<Point_2>();
            cusp = tpie::deque<Point_2>();
        }

        inline void extend(Edge e){
            Point_2 a = e.a;
            Point_2 b = e.b;

            // one of the points should already be part of the cusp
            if (a == cusp.peekFront()){
                extend(b, false);
            } else if (a == cusp.peekBack()){
                extend(b, true);
            } else if (b == cusp.peekFront()){
                extend(a, false);
            } else if (b == cusp.peekBack()){
                extend(a, true);
            }
        }

        inline void extendFinalStep(Point_2 target){
            // This is a little bit backwards because points have to be added in the right order
            // So first skip through the "wrong" side till we find the apex then add the rest
            bool apexFound = false;
            if (target.distance(cusp.peekFront()) < target.distance(cusp.peekBack()))
            {
                extend(target, true);
                Point_2 p;
                while (cusp.size()){
                    p = cusp.popBack();

                    if (p == apex) apexFound = true;

                    if (apexFound) geodesic.push(p);
                }
            }
            else
            {
                extend(target, false);
                Point_2 p;
                while (cusp.size()){
                    p = cusp.popFront();

                    if (p == apex) apexFound = true;

                    if (apexFound) geodesic.push(p);
                }
            }
        }

        inline void extend(Point_2 p, bool front) {

            Point_2 a, b;
            bool passedApex = false;

            if (front){
                while(true){
                    a = cusp.popFront();

                    if (!cusp.empty()){
                        b = cusp.peekFront();
                    } else {
                        // Rare case if a new point is added that is only tangent with the very last point
                        cusp.pushFront(a);
                        apex = cusp.peekFront();
                        cusp.pushFront(p);
                        break;
                    }

                    if (a == apex) passedApex = true;

                    if (!passedApex){
                        if (leftTurn(p, a, b)){
                            cusp.pushFront(a);
                            cusp.pushFront(p);
                            break;
                        }
                    } else {
                        if (rightTurn(p, a, b)){
                            cusp.pushFront(a);
                            apex = cusp.peekFront();
                            cusp.pushFront(p);
                            break;
                        } else {
                            geodesic.push(a);
                        }
                    }
                }
            } else {
                while(true){
                    a = cusp.popBack();

                    if (!cusp.empty()){
                        b = cusp.peekBack();
                    } else {
                        // Rare case if a new point is added that is only tangent with the very last point
                        cusp.pushBack(a);
                        apex = cusp.peekBack();
                        cusp.pushBack(p);
                        break;
                    }

                    if (a == apex) passedApex = true;

                    if (!passedApex){
                        if (rightTurn(p, a, b)){
                            cusp.pushBack(a);
                            cusp.pushBack(p);
                            break;
                        }
                    } else {
                        if (leftTurn(p, a, b)){
                            cusp.pushBack(a);
                            apex = cusp.peekBack();
                            cusp.pushBack(p);
                            break;
                        } else {
                            geodesic.push(a);
                        }
                    }
                }
            }
        }

        inline tpie::stream_size_type getStackSize(){
            return geodesic.size();
        }

        inline tpie::stack<Point_2> * getStack(){
            return &geodesic;
        }

        inline tpie::deque<Point_2> getCusp(){
            return cusp;
        }

    protected:

    private:
        tpie::stack<Point_2> geodesic;
        tpie::deque<Point_2> cusp;
};

#endif // FUNNEL_H
