#ifndef BASE_FUNNEL_H
#define BASE_FUNNEL_H

#include <definition.h>

class base_funnel
{
    public:

        inline bool leftTurn(const Point_2 & p1, const Point_2 & p2, const Point_2 & p3) {
            return (p1.x-p3.x)*(p2.y-p3.y) - (p1.y-p3.y)*(p2.x-p3.x) < 0;
        }

        inline bool rightTurn(const Point_2 & p1, const Point_2 & p2, const Point_2 & p3) {
            return leftTurn(p3, p2, p1);
        }

        inline Point_2 getApex(){
            return apex;
        }

    protected:
        Point_2 apex;

    private:

};

#endif // BASE_FUNNEL_H
