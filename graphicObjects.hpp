#include "Image.h"

struct Point3 {
    double x, y, z;

    Point3() {x = y = z = 0;}
    Point3(int x1) {x = x1; y = z = 0;} 
};

class Camera {
   Point3 focus;
   Image matrix;

public:
    Camera(int resX = 1024, int resY = 1024, int a_channels = 4, Point3 foc_pos = 0);
};

class GraphObject {

};

class Emitter : public GraphObject {

};

class Ray {

};
    
    
    