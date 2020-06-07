#ifndef FUNNEL_H
#define FUNNEL_H

#include <definition.h>
#include <tpie/stack.h>
#include <deque.h>


class funnel
{
    public:
        funnel(Edge e) {
            Point_2 source = e.a;
            Point_2 p = e.b;

            cusp = tpie::deque<Point_2>();
            cusp.pushFront(source);
            apex = cusp.peekFront();

            if (p.x < source.x) cusp.pushFront(p);
            else cusp.pushBack(p);

            geodesic = tpie::stack<Point_2>();
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

        inline bool leftTurn(const Point_2 & p1, const Point_2 & p2, const Point_2 & p3) {
            return (p1.x-p3.x)*(p2.y-p3.y) - (p1.y-p3.y)*(p2.x-p3.x) < 0;
        }

        inline bool rightTurn(const Point_2 & p1, const Point_2 & p2, const Point_2 & p3) {
            return leftTurn(p3, p2, p1);
        }

        inline tpie::stack<Point_2> getStack(){
            return geodesic;
        }

        inline Point_2 getApex(){
            return apex;
        }

        inline tpie::deque<Point_2> getCusp(){
            return cusp;
        }

    protected:

    private:
        Point_2 apex;
        tpie::stack<Point_2> geodesic;
        tpie::deque<Point_2> cusp;
};

#endif // FUNNEL_H
