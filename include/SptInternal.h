#ifndef SPTINTERNAL_H
#define SPTINTERNAL_H

#include <base_funnel.h>
#include <time.h>


struct sptResultPoint {
    Point_2 point;
    int parent;
};

struct SptHistoryNode{
    bool add, front;
    sptResultPoint changedPoint;
    Point_2 apex;
    int index;
};

class SptInternal: public base_funnel
{
    public:
        SptInternal(size_t size)
        {
            cusp = tpie::internal_deque<sptResultPoint>(2 * size);
            count = 0;
            history = tpie::internal_stack<SptHistoryNode>(size);
            result.resize(size+1);
            point2Zero = {0,0};
            srand(time(NULL));
        }

        inline void addHistory(bool front, sptResultPoint changedPoint){
            SptHistoryNode shn;
            shn.add = true;
            shn.front = front;
            shn.changedPoint = changedPoint;

            history.push(shn);
        }

        inline void addHistory(bool front, int index){
            SptHistoryNode shn;
            shn.add = false;
            shn.front = front;
            shn.index = index;
            shn.apex = apex;

            history.push(shn);
        }

        inline void undo(){
            SptHistoryNode shn = history.top();
            history.pop();

            // if it's an add or split we're undoing
            if (shn.add){
                cusp.undoAdd(shn.front, shn.changedPoint);
            } else {
                cusp.undoSplit(shn.front, shn.index);
                apex = shn.apex;
            }
        }

        inline void extendStart(Node source, Edge e){
            // source = apex should be the point of source triangle that is not on the first added edge
            for (int i = 0; i < 3; i++){
                apex = source.points[i];
                if (!(apex == e.a) && !(apex == e.b)) break;
            }
            addHistory(true, cusp.index(cusp.first - 1));
            cusp.pushFront({apex, count});
            result[count] = {apex, -1};
            count++;

            if (rightTurn(e.a, apex, e.b)){
                addHistory(true, cusp.index(cusp.first - 1));
                cusp.pushFront({e.a, count});
                result[count] = {e.a, 0};
                count++;

                addHistory(false, cusp.index(cusp.last + 1));
                cusp.pushBack({e.b, count});
                result[count] = {e.b, 0};
                count++;
            } else {
                addHistory(true, cusp.index(cusp.first - 1));
                cusp.pushFront({e.b, count});
                result[count] = {e.b, 0};
                count++;

                addHistory(false, cusp.index(cusp.last + 1));
                cusp.pushBack({e.a, count});
                result[count] = {e.a, 0};
                count++;
            }
            cusp.print();
        }

        inline void extend(Edge e){
            Point_2 a = e.a;
            Point_2 b = e.b;

            // if we get boges Edge it means we should undo twice
            if (a == point2Zero && b == point2Zero){
                std::cout << "Undoing twice" << std::endl;
                undo();
                undo();
            }
            else
            {
                // one of the points should already be part of the cusp
                if (a == cusp.peekFront().point){
                    extend(b, false);
                } else if (a == cusp.peekBack().point){
                    extend(b, true);
                } else if (b == cusp.peekFront().point){
                    extend(a, false);
                } else if (b == cusp.peekBack().point){
                    extend(a, true);
                } else {
                    std::cout << a.x << " " << a.y << " " << b.x << " " << b.y << std::endl;
                    std::cout << cusp.peekFront().point.x << " " << cusp.peekFront().point.y << " " << cusp.peekBack().point.x << " " << cusp.peekBack().point.y << std::endl;
                }
            }

            cusp.print();
        }

        inline void extend(Point_2 p, bool front) {
            std::cout << "10" << std::endl;
            sptResultPoint a, b;
            bool passedApex = false;
            int i = 0;

            if (front){
                while(true){
                    a = cusp.index(cusp.first + i);

                    if (cusp.size() > i + 1){
                        b = cusp.index(cusp.first + i + 1);
                    } else {
                        std::cout << "1" << std::endl;
                        // Rare case if a new point is added that is only tangent with the very last point
                        addHistory(true, cusp.first);
                        cusp.split(true, i);
                        addHistory(true, cusp.index(cusp.first - 1));
                        cusp.pushFront({p, count});
                        result[count] = {p, a.parent};
                        count++;
                        apex = a.point;
                        break;
                    }

                    if (a.point == apex) passedApex = true;

                    if (!passedApex){
                        if (leftTurn(p, a.point, b.point)){
                            std::cout << "2" << std::endl;
                            addHistory(true, cusp.first);
                            cusp.split(true, i);
                            addHistory(true, cusp.index(cusp.first - 1));
                            cusp.pushFront({p, count});
                            result[count] = {p, a.parent};
                            count++;
                            break;
                        }
                    } else {
                        if (rightTurn(p, a.point, b.point)){
                            std::cout << "3" << std::endl;
                            addHistory(true, cusp.first);
                            cusp.split(true, i);
                            addHistory(true, cusp.index(cusp.first - 1));
                            cusp.pushFront({p, count});
                            result[count] = {p, a.parent};
                            count++;
                            apex = a.point;
                            break;
                        }
                    }
                    i++;
                }
            } else {
                while(true){
                    a = cusp.index(cusp.last - i);

                    if (cusp.size() > i + 1){
                        b = cusp.index(cusp.last - i - 1);
                    } else {
                        // Rare case if a new point is added that is only tangent with the very last point
                        std::cout << "5" << std::endl;
                        addHistory(false, cusp.last);
                        cusp.split(false, i);
                        addHistory(false, cusp.index(cusp.last + 1));
                        cusp.pushBack({p, count});
                        result[count] = {p, a.parent};
                        count++;
                        apex = a.point;
                        break;
                    }

                    if (a.point == apex) passedApex = true;

                    if (!passedApex){
                        if (rightTurn(p, a.point, b.point)){
                            std::cout << "6" << std::endl;
                            addHistory(false, cusp.last);
                            cusp.split(false, i);
                            addHistory(false, cusp.index(cusp.last + 1));
                            cusp.pushBack({p, count});
                            result[count] = {p, a.parent};
                            count++;
                            break;
                        }
                    } else {
                        if (leftTurn(p, a.point, b.point)){
                            std::cout << "7" << std::endl;
                            addHistory(false, cusp.last);
                            cusp.split(false, i);
                            addHistory(false, cusp.index(cusp.last + 1));
                            cusp.pushBack({p, count});
                            result[count] = {p, a.parent};
                            count++;
                            apex = a.point;
                            break;
                        }
                    }
                    i++;
                }
            }
        }

        inline void print(){
            for (int i = 0; i < result.size(); i++){
                std::cout << i << " " << result[i].point.x << " " << result[i].point.y << " " << result[i].parent << std::endl;
            }
        }

        inline void generateRandomTargets(int count){
            targets.resize(count);

            for (int i = 0; i < count; i++){
                targets[i] = rand() % result.size();
            }
        }

        inline std::vector<Point_2> queryTarget(int i){
            std::vector<Point_2> path = std::vector<Point_2>();

            if (i < targets.size()){
                sptResultPoint srp = result[targets[i]];
                while (srp.parent != -1){
                    path.push_back(srp.point);
                    srp = result[srp.parent];
                }
                path.push_back(result[0].point);
                return path;
            } else {
                return path;
            }
        }

        inline int targetCount(){
            return targets.size();
        }


    protected:

    private:
        int count;
        tpie::internal_deque<sptResultPoint> cusp;
        tpie::internal_stack<SptHistoryNode> history;
        tpie::array<sptResultPoint> result;
        tpie::array<int> targets;
        Point_2 point2Zero;
};

#endif // SPTINTERNAL_H
