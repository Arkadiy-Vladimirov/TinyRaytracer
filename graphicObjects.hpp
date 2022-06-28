#ifndef __grObj_hpp__
#define __grObj_hpp__

#include "Image.h"
#include "lin_al.hpp"
#include "interactionModels.hpp"
#include <cmath>

#define PI 3.14159265

typedef Vec<2,unsigned> Vec2un;
typedef Vec<2,float> Vec2f;
typedef Vec<3,double> Color;//changed
typedef Vec<3,float> Vec3f;

class GrObjCollection;
class GraphObject;
class Ray;
class MediaInteractionModel;

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
                                                            int a_channels = 4, Vec2f f_mat_size = Vec2f(10,10)); //warning: channels!
    const Image& RenderImage(const GrObjCollection& scene);
            void RenderPixel(const GrObjCollection& scene, unsigned x, unsigned y);

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

    Color Cast(const GrObjCollection& scene) const;
    const GraphObject* HittedObjectPtr(const GrObjCollection& scene, Vec3f& hit_point) const; //NULL means no hit

    Vec3f        GetOrigin()         const {return orig;            };
    Vec3f        GetDirection()      const {return direction;       };
    unsigned     GetRecursionDepth() const {return recursion_depth; };

    static float GetEpsilon()              {return epsilon;         };
};

class GrObjCollection {
    unsigned size;
    GraphObject** data;
public:
    GrObjCollection() : size(0) {data = new GraphObject*[size];}
    ~GrObjCollection();

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
    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const = 0;

    Repere GetLocation() const {return location; };
};

class Polygon: public GraphObject {
    Vec3f vertices[3];
public:
    Polygon(const Vec3f* vert);
    virtual ~Polygon() {};

    virtual bool CheckHit(const Ray& ray, Vec3f& hit_point) const;
    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const;
};

class PolygonMesh : public GraphObject { //theoretically may be used for arbitrary collection of objects
    GrObjCollection polygons;
    const MediaInteractionModel* material;
    float scale;

    static const GraphObject* hitted_polygon; //a bit weird solution
public:
    PolygonMesh(const Repere& f_loc, const MediaInteractionModel* mat, Vec3f* vert_buf, unsigned buf_size, float scal = 1);
    virtual ~PolygonMesh() {};

    virtual bool CheckHit(const Ray& ray, Vec3f& hit_point) const; 
    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const;

    Vec3f GetNormal(const Vec3f& dot) const;

    static void SetHittedPolygon(const GraphObject* h_p) {hitted_polygon = h_p; };
    static const GraphObject* GetHittedPolygon() {return hitted_polygon; };
};

class Ball : public GraphObject {
    float radius;
    const MediaInteractionModel* material;
public:
    Ball(const Repere& f_loc, const MediaInteractionModel* mat, float f_rad = 1) : GraphObject(f_loc), material(mat), radius(f_rad) {};
    virtual ~Ball() {};

    virtual bool CheckHit(const Ray& ray, Vec3f& hit_point) const;
    virtual Color Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const;

    float GetRadius() const {return radius; };
    Vec3f GetNormal(const Vec3f& hit_point) const {return normalize(hit_point - location.orig); }; //returns outer normal
};


#endif //__grObj_hpp__    
    
