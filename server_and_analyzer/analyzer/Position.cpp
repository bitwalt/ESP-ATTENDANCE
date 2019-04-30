#include "Position.h"
#include <vector>

Position::Position(const vector<double>& distances, vector<Position*>& esp_pos ) {

    long N = esp_pos.size();
    double x_dim = esp_pos[1]->getX();
    double x3,y3;
    double new_x, new_y, q;
    double dist0, dist1, temp;

    switch (N) {

    case 2:
        new_x = (distances[0] + x_dim - distances[1]) / 2;
        if (new_x < 0 || new_x > x_dim)
            inside = false;

        this->x = new_x;
        this->y = 0;
        break;

    case 3:
        double m, dist;

        x3 = esp_pos[2]->getX();
        y3 = esp_pos[2]->getY();

        dist0 = (int) distances[0];
        dist1 = (int) distances[1];

        if(x_dim == 0) {
            temp = dist0;
            dist0 = dist1;
            dist1 = temp;
            x_dim = esp_pos[1]->getY();
        }

        new_x = calculateXY(dist0, dist1, x_dim);
        new_y = sqrt(abs((-new_x + dist1 + x_dim)) * abs((new_x + dist1 - x_dim)));

        if (x3 == 0) {

            m = -(y3 / x_dim);
            q = y3;
            dist = new_y - (m * new_x + q);
            if (dist > 0)
                inside = false;
        } else if (x3 == x_dim) {
            m = y3 / x3;
            dist = new_y - (m * new_x);
            if (dist > 0)
                inside = false;
        } else {
            if (new_x < x3) {
                double m1 = y3 / x3;
                dist = new_y - (m1 * new_x);
                if (dist > 0)
                    inside = false;
            } else {
                double m2 = y3 / (x3 - x_dim);
                q = -((x_dim) * x3 / (x3 - x_dim));
                dist = new_y - (m2 * new_x + q);
                if (dist > 0)
                    inside = false;
            }
        }

        this->x = new_x;
        this->y = new_y;
        break;

    case 4:
        int y_dim = esp_pos[2]->getY();

        //le 2 schede in basso
        double x1 = calculateXY( distances[0],  distances[1], x_dim);
        double y1 = sqrt(abs((-x1+distances[1]+x_dim))*abs((x1+distances[1]-x_dim)));
        //le due schede in alto
        double x2 = x_dim - calculateXY( distances[2], distances[3], x_dim);
        double y2 = y_dim - sqrt(abs((-x1+distances[3]+x_dim))*abs((x1+distances[3]-x_dim)));
        //le due schede a destra
        double my3 = calculateXY( distances[1], distances[2], y_dim);
        double mx3 = y_dim -  sqrt(abs((-x1+distances[2]+x_dim))*abs((x1+distances[2]-x_dim)));
        //le due schede a sinistra
        double y4 = y_dim - calculateXY( distances[3], distances[0], y_dim);
        double x4 =  sqrt(abs((-x1+distances[0]+x_dim))*abs((x1+distances[0]-x_dim)));

        new_x = avg(x1,x2,mx3,x4);
        new_y = avg(y1,y2,my3,y4);

        if(new_x < 0 || new_x > x_dim)
            inside = false ;
        if(new_y < 0 || new_y > y_dim)
            inside = false;

        this->x = new_x;
        this->y = new_y;
        break;
    }
}

double Position::calculateXY(double a, double d, double x_y){
    double a_squared = pow(a, 2);
    double d_squared = pow(d, 2);
    double xy_squared = pow(x_y,2);
    return (a_squared-d_squared+xy_squared)/(2*x_y);
}

bool Position::isInside() const {
    return inside;
}
