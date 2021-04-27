#ifndef __interMod_hpp__
#define __interMod_hpp__

#include "lin_al.hpp"
#include "graphicObjects.hpp"
#include <cmath>

//fixes_due_to_strange_linking_problem_
typedef Vec<3,double> Color;
class GrObjCollection;
class GraphObject;
class Ray;
//_____________________________________

class MediaInteractionModel {
public:
                  MediaInteractionModel() {};
    virtual      ~MediaInteractionModel() {};

    virtual Color Interact(const Ray& ray, const Vec3f& hit_point, const Vec3f& outer_normal, const GrObjCollection& scene) const = 0;
};

class SimpleEmission : public MediaInteractionModel {
            Color emission;
public:
                  SimpleEmission(const Color& emis = Color()) : emission(emis) {};
    virtual      ~SimpleEmission() {};

    virtual Color Interact(const Ray& ray, const Vec3f& hit_point, const Vec3f& outer_normal, const GrObjCollection& scene) const;
};

class Lambert : public MediaInteractionModel {
           Color    color;
           float    diffusivity; //not realized yet

    static unsigned dispersed_rays_number; //number of rays diffusively reflected at each hit
public:
                  Lambert(Color col = Color(), float dif = 1) : color(col), diffusivity(dif) {};
    virtual      ~Lambert() {};

    virtual Color Interact(const Ray& ray, const Vec3f& hit_point, const Vec3f& outer_normal, const GrObjCollection& scene) const;

            float GetDispersedDirections(const Repere& local_basis, float incident_angle, Vec3f* dir_arr, float* cos_arr, unsigned size) const;

            Color    GetReflectanceSpectrum() const {return Color(color.x/255, color.y/255, color.z/255); };
    static  unsigned GetDispersedRaysNumber()       {return dispersed_rays_number; };
};

class Schlick : public MediaInteractionModel {
    float inner_refractive_index;
    float outer_refractive_index;
    Color color;
public:
                  Schlick(float f_in_r = 1.5, float f_out_r = 1, Color col = Color(255,255,255)) : inner_refractive_index(f_in_r), outer_refractive_index(f_out_r), color(col) {};
    virtual      ~Schlick() {};

    virtual Color Interact(const Ray& ray, const Vec3f& hit_point, const Vec3f& outer_normal, const GrObjCollection& scene) const;

    void SetConfig(const Vec3f& dir, const Vec3f& dot, const Vec3f& outer_normal, Vec3f& normal, float& n1, float& n2, float& alpha, Color& abs_coef) const;
    
    Color GetAbsorbtionCoefficient() const {return Color(1,1,1) - 1/double(255) * color; }


};

#endif //__interMod_hpp__