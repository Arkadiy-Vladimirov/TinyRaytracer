#ifndef __grObj_hpp__
#define __grObj_hpp__

#include "Image.h"
#include "lin_al.hpp"
#include <cmath>

#define PI 3.14159265

typedef Vec<2,unsigned> Vec2un;
typedef Vec<2,float> Vec2f;
typedef Vec<3,double> Color;//changed
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
    static unsigned sqrt_pix_rays; // =sqrt of number of rays casted per pixel
public:
    Camera(Vec3f f_origin, Vec3f view_dir, double f_fov = PI/2, Vec2un resolution = Vec2un(1024, 1024),
                                                            int a_channels = 3, Vec2f f_mat_size = Vec2f(10,10)); //warning: channels!
    const Image& RenderImage(const Scene& scene);
            void RenderPixel(const Scene& scene, unsigned x, unsigned y);

           Vec3f GetPixelLocation(unsigned x, unsigned y) const {return mat_base.orig +  x*mat_base.e1 + y*mat_base.e2; };
    static float GetRenderDistance()                            {return render_distance; };

    //Vec2un MatrixRes() const {return Vec2un(matrix.Width(), matrix.Height());}; 
};

class Ray {
    Vec3f orig;
    Vec3f direction;
    unsigned recursion_depth;

    static float epsilon;
    static unsigned max_recursion_depth;
public:
    Ray(Vec3f f_orig, Vec3f f_dir, unsigned f_rec_d = 0) {orig = f_orig; direction = normalize(f_dir); recursion_depth = f_rec_d; };

    Color Cast(const Scene& scene) const;
    const GraphObject* HittedObjectPtr(const Scene& scene, Vec3f& hit_point) const; //NULL means no hit

    Vec3f        GetOrigin()         const {return orig;            };
    Vec3f        GetDirection()      const {return direction;       };
    unsigned     GetRecursionDepth() const {return recursion_depth; };

    static float GetEpsilon()              {return epsilon;         };
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
    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const = 0;

    Repere GetLocation() const {return location; };
};

class Polygon: public GraphObject {
    Vec3f vertices[3];
public:
    Polygon(Vec3f* vert);
    virtual ~Polygon() {};

    virtual bool CheckHit(const Ray& ray, Vec3f& hit_point) const;
    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const;
};

class PolygonMesh : public GraphObject {
    //obj_collections

public:
    PolygonMesh(const Repere& f_loc = Repere()) : GraphObject(f_loc) {};
    virtual ~PolygonMesh() {};

    virtual bool CheckHit(const Ray& ray, Vec3f& hit_point) const = 0;
    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const = 0;
};

class Ball : public GraphObject {
protected:
    float radius;
public:
    Ball(const Repere& f_loc = Repere(), float f_rad = 1) : GraphObject(f_loc), radius(f_rad) {};
    virtual ~Ball() {};

    virtual bool CheckHit(const Ray& ray, Vec3f& hit_point) const;
    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const = 0;

    float GetRadius() const {return radius; };
    Vec3f GetNormal(const Vec3f& hit_point) const {return normalize(hit_point - location.orig); }; //returns outer normal
};

//_____GrObj______
//_____Ball_______
//_GlassBall______
//_DiffuseBall____
//_EmittingBall___

class RefractiveBall : public Ball {
    float inner_refractive_index;
    float outer_refractive_index;
    //float absorbtion_coefficient?
public:
    RefractiveBall(const Repere& f_loc = Repere(), float f_rad = 1, float f_in_r = 1.5, float f_out_r = 1) : Ball(f_loc,f_rad), inner_refractive_index(f_in_r), outer_refractive_index(f_out_r) {};
    virtual ~RefractiveBall() {};

    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const;

    void SetConfig(const Vec3f& dir, const Vec3f& dot, Vec3f& normal, float& n1, float& n2, float& alpha) const;

};

class DiffuseBall : public Ball {
    Color color;
    float diffusivity;

    static unsigned dispersed_rays_number; //number of rays diffusively reflected at each hit
public:
            DiffuseBall(const Repere& f_loc = Repere(), float f_rad = 1, Color f_col = Color(), float dif = 1) : Ball(f_loc,f_rad), color(f_col), diffusivity(dif) {};
    virtual ~DiffuseBall() {};

    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene)   const;

            float GetDispersedDirections(const Repere& local_basis, float incident_angle, Vec3f* dir_arr, float* cos_arr, unsigned size) const;

            Color    GetReflectanceSpectrum() const {return Color(color.x/255, color.y/255, color.z/255); };
    static  unsigned GetDispersedRaysNumber()       {return dispersed_rays_number; };
};

class EmittingBall : public Ball{
    Color emission;
public:
    EmittingBall(const Repere& f_loc = Repere(), float f_rad = 1, const Color& f_emis = Color()) : Ball(f_loc,f_rad), emission(f_emis) {};
    virtual ~EmittingBall() {};

    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const;

    Color GetEmission() const {return emission; };
};

#endif //__grObj_hpp__    
    
