#ifndef __grObj_hpp__
#define __grObj_hpp__

#include "Image.h"
#include "lin_al.hpp"
#include <cmath>

#define PI 3.14159265

typedef Vec<2,unsigned> Vec2un;
typedef Vec<2,float> Vec2f;
typedef Vec<3,uint8_t> Color;
typedef Vec<3,float> Vec3f;

class Scene;
class GraphObject; 

class Camera {
    Repere cam_base; //e1 - fwd, e2 - lft, e3 - top
    double fov;                  //degrees
    Image matrix;
    Vec2f matrix_size; //cm
    
    //redundant but helpful data
    Repere mat_base; //origin, e1 = x_base to shift one pixel to the positive x direction in matrix, e2 = y_base --||--.
    Vec2f pixel_size; //cm

public:
    Camera(Vec3f f_origin, Vec3f view_dir, double f_fov = PI/2, Vec2un resolution = Vec2un(1024, 1024),
                                                            int a_channels = 3, Vec2f f_mat_size = Vec2f(10,10)); //warning: channels!
    const Image& RenderImage(const Scene& scene);

    //Vec2un MatrixRes() const {return Vec2un(matrix.Width(), matrix.Height());}; 
};

class Ray {
    //fields
public:
    Ray(Vec3f f_orig, Vec3f f_dir);
    Color Cast();
};

class Scene { //use carefully
    unsigned size;
    GraphObject** data;
public:
    Scene(unsigned f_size) : size(f_size) {data = new GraphObject*[f_size];}
    ~Scene();//u.c.            

    GraphObject*& operator[](unsigned idx); //u.c.
};

class GraphObject {
protected:
    Repere location;
public:
    GraphObject(const Repere& f_loc = Repere()) : location(f_loc) {};
    virtual ~GraphObject() {};
};

class MonochromeSphere : public GraphObject {
    float radius;
    Color color;
public:
    MonochromeSphere(const Repere& f_loc = Repere(), float f_rad = 1, const Color& f_col = Color()) : GraphObject(f_loc), radius(f_rad), color(f_col) {};
    virtual ~MonochromeSphere() {};
};

/*class Emitter : public GraphObject {

};*/



    
#endif //__grObj_hpp__    
    
