#include "Image.h"

struct Vec2 {
    double x, y;

    Vec2(double x1 = 0, double x2 = 0) {x = x1; y = x2;}
};
struct Vec3 : Vec2{
    double z;

    Vec3(double x1 = 0, double x2 = 0, double x3 = 0) : Vec2(x1, x2) {z = x3;} 
};

class Camera { //view direction?
    Vec3 focus;
    Vec3 view_direction;
    Vec2 fov; //degrees
    Image matrix;

public:
    Camera(Vec3 foc_pos, Vec3 v_dir, Vec2 fov = Vec2(90,60), Vec2 resolution = Vec2(1024, 1024), int a_channels = 4);
};

class GraphObject {

};

class Emitter : public GraphObject {

};

class Ray {

};
    
    
    