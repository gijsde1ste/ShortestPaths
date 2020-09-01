#ifndef DEFINITION_H
#define DEFINITION_H

#include <math.h>
#include <string>

#include <tpie/tpie.h>
#include <tpie/tpie_log.h>
#include <tpie/sort.h>
#include <tpie/stack.h>

const double epsilon = 0.00000000001;

struct Point_2 {
    double x, y;

    bool operator==(const Point_2 & other) const {
        return (fabs(x - other.x) <= epsilon && fabs(y - other.y) <= epsilon);
	}

	// returns distance^2, good enough to compare distance without using sqrt
	inline double distance(const Point_2 & other) const {
        return pow(other.x-x, 2) + pow(other.y-y, 2);
	}
};

struct Edge{
    Point_2 a, b;
};

const Edge edgeZero = Edge{{0,0}, {0,0}};

struct Triangle{
    Point_2 a, b, c;
};

#endif // DEFINITION_H
