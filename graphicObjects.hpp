#include "Image.h"
#include "lin_al.hpp"
#include <cmath>

#define PI 3.14159265

typedef Vec<2,unsigned> Vec2un;
typedef Vec<3,float> Vec3f;
typedef Vec<2,float> Vec2f;

class Camera {
    Vec3f focus;
    Vec3f orientation[3]; //view_dir, top_dir, right_dir
    double fov;                  //degrees
    Image matrix;
    Vec2f matrix_size; //cm
    
    //redundant but helpful data
    Vec3f matrix_origin; // (x,y) = (0,0)
    Vec2f pixel_size; //cm
    Vec3f x_base; //to shift one pixel to the positive x direction in matrix
    Vec3f y_base; //----||---- y direction

public:
    Camera(Vec3f f_focus, Vec3f[3] f_orient, double f_fov = PI/2, Vec2un resolution = Vec2un(1024, 1024),
                                                            int a_channels = 3, Vec2f f_mat_size = Vec2f(10,10)); //warning: channels!
    Image* RenderImage();
};

/*class GraphObject {

};

class Emitter : public GraphObject {

};

class Ray {

};*/
    
    
    
