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
class Ray;

class Camera {
    Repere cam_base; //e1 - fwd, e2 - lft, e3 - top
    double fov;                  //degrees
    Image matrix;
    Vec2f matrix_size; //cm
    
    //redundant but helpful data
    Repere mat_base; //origin, e1 = x_base to shift one pixel to the positive x direction in matrix, e2 = y_base --||--.
    Vec2f pixel_size; //cm

    static float render_distance;

public:
    Camera(Vec3f f_origin, Vec3f view_dir, double f_fov = PI/2, Vec2un resolution = Vec2un(1024, 1024),
                                                            int a_channels = 3, Vec2f f_mat_size = Vec2f(10,10)); //warning: channels!
    const Image& RenderImage(const Scene& scene);

    Vec3f GetPixelLocation(unsigned x, unsigned y) const {return mat_base.orig +  x*mat_base.e1 + y*mat_base.e2;};
    static float GetRenderDistance() {return render_distance;};

    //Vec2un MatrixRes() const {return Vec2un(matrix.Width(), matrix.Height());}; 
};

class Ray {
    Vec3f orig;
    Vec3f direction;
public:
    Ray(Vec3f f_orig, Vec3f f_dir) {orig = f_orig; direction = normalize(f_dir); };

    Color Cast(const Scene& scene) const;
    const GraphObject* HittedObjectPtr(const Scene& scene) const; //NULL means no hit

    Vec3f GetOrigin() const {return orig;};
    Vec3f GetDirection() const {return direction;};
};

class Scene {
    unsigned size;
    GraphObject** data;
public:
    Scene() : size(0) {data = new GraphObject*[size];}
    ~Scene();

    unsigned GetSize() const {return size;};
    const GraphObject*  operator[](unsigned idx) const; //to acces existing object
          GraphObject*& operator[](unsigned idx);       //to add new object
};

class GraphObject {
protected:
    Repere location;
public:
    GraphObject(const Repere& f_loc = Repere()) : location(f_loc) {};
    virtual ~GraphObject() {};

    virtual bool CheckHit(const Ray& ray, Vec3f& hit_point) const = 0;
    virtual Color Hit(const Ray& ray) const = 0;

    Repere GetLocation() const {return location; };
};

class Ball : public GraphObject {
protected:
    float radius;
public:
    Ball(const Repere& f_loc = Repere(), float f_rad = 1) : GraphObject(f_loc), radius(f_rad) {};
    virtual ~Ball() {};

    virtual bool CheckHit(const Ray& ray, Vec3f& hit_point) const;
    virtual Color Hit(const Ray& ray) const = 0;

    float GetRadius() const {return radius; };
};

//_____GrObj______
//_____Ball_______
//_GlassBall______
//_DiffuseBall____
//_EmittingBall___

class EmittingBall : public Ball{
    Color emition;
public:
    EmittingBall(const Repere& f_loc = Repere(), float f_rad = 1, const Color& f_emit = Color()) : Ball(f_loc,f_rad), emition(f_emit) {};
    virtual ~EmittingBall() {};

    virtual Color Hit(const Ray& ray) const;

    Color GetEmition() const {return emition; };
};

#endif //__grObj_hpp__    
    
