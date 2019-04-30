#ifndef SERVER2_POSITION_H
#define SERVER2_POSITION_H

#include <iostream>
#include <vector>
#include "math.h"

using namespace std;
class Position {

private:
    double x, y;
    bool inside = true;

public:
    bool isInside() const;
    double calculateXY(double, double, double);
    Position(const vector<double>&, vector<Position*>&);

    Position(double x, double y) {
        this->x = x;
        this->y = y;
    }

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    double avg(double a1, double a2, double a3, double a4 ){
        return (a1+a2+a3+a4)/4;
    }
};

#endif //SERVER2_POSITION_H
