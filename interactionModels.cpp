#ifndef __interMod_cpp__
#define __interMod_cpp__

#include "interactionModels.hpp"
#include "lin_al.hpp"
#include "graphicObjects.hpp"
#include <cmath>
#include <stdio.h>

unsigned Lambert::dispersed_rays_number = 8;

Color SimpleEmission::Interact(const Ray& ray, const Vec3f& hit_point, const Vec3f& outer_normal, const GrObjCollection& scene) const {
    return emission;
};

Color Lambert::Interact(const Ray& ray, const Vec3f& hit_point, const Vec3f& outer_normal, const GrObjCollection& scene) const {
    Vec3f n = outer_normal, r = ray.GetDirection();
    float alpha = angle(-n,r);
    Vec3f tau = r + cos(alpha) * n;
    Repere tan_bas(hit_point,n,tau,true); //e1 - normal vector, e2 - tangent vector, e3 - cross product

    Color resCol(0,0,0); //black(no light)
    unsigned N = GetDispersedRaysNumber();
    Color RS = GetReflectanceSpectrum();
    
    Vec3f disp_dirs[N]; float COS[N]; //cos beetwen normal and disp_dir[i]
    float R = GetDispersedDirections(tan_bas,alpha,disp_dirs,COS,N);

    for (int i = 0; i < N ; ++i) {
        Ray reflected_ray(tan_bas.orig + Ray::GetEpsilon()*disp_dirs[i], disp_dirs[i], ray.GetRecursionDepth() + 1);
        resCol = resCol + R*COS[i]*RS*reflected_ray.Cast(scene);
    }

    return resCol;
};

float Lambert::GetDispersedDirections(const Repere& local_basis, float incident_angle, Vec3f* dir_arr, float* cos_arr, unsigned size) const {
    float phi, theta;
    float cos_sum = 0; 
    for (int i = 0; i < size; ++i) {
         phi  = float(std::rand());  phi  =  phi  / RAND_MAX * (2*PI);   // [0,pi], E(phi) = pi/2 - alpha 
        theta = float(std::rand()); theta = theta / RAND_MAX * (PI/2);   // [0,pi], E(thetha) = pi/2
        //polar transform: e1 = z, e2 = x, e3 = y;
        dir_arr[i] = sin(theta)*cos(phi)*local_basis.e2 + sin(theta)*sin(phi)*local_basis.e3 + cos(theta)*local_basis.e1;
        cos_arr[i] = cos(local_basis.e1,dir_arr[i]);
        cos_sum += cos_arr[i];
    }
    return 1/cos_sum;
}; 


Color Schlick::Interact(const Ray& ray, const Vec3f& hit_point, const Vec3f& outer_normal, const GrObjCollection& scene) const {
    //determine where are we
    float n1, n2; //refractive indices 1 - before surface, 2 - after surface.
    float alpha;  //normal angle in rad < pi/2.
    Vec3f n;      //surface normal, direction: 2->1
    Vec3f r = ray.GetDirection();
    Color abs_coef;
    SetConfig(r,hit_point,outer_normal,  n,n1,n2,alpha,abs_coef);

    //calculate direction of reflection(angle of incidence equals to angle of reflection)
    Vec3f reflect_dir = normalize(r - 2*scalar(n,r)*n);
    Ray reflected_ray(hit_point + Ray::GetEpsilon()*reflect_dir, reflect_dir, ray.GetRecursionDepth() + 1);


    //total internal reflection check 
    if (n1*sin(alpha) > n2) {
        return reflected_ray.Cast(scene);
    }
    
    //calculate direction of refraction(Snell's law)
    Vec3f refract_dir = normalize( (n1*r + (sqrt( (n2*n2 -n1*n1)/pow(scalar(n1*r,n),2) + 1 )  -  1) * scalar(n1*r,n) * n) );
    Ray refracted_ray(hit_point + Ray::GetEpsilon()*refract_dir, refract_dir, ray.GetRecursionDepth() + 1);

    //calculate intensivities(Schlick's approximation)
        float R0 = pow(((n1-n2)/(n1+n2)),2);
        float R = R0 + (1-R0)*pow((1-cos(alpha)),5); //intensivity of reflected ray 
    
    //calculate absorbation
    double distance = norm(hit_point-ray.GetOrigin());
    Color absorbation = distance*abs_coef;

    //total internal absorbation check
    if (absorbation.x > 1) { absorbation.x = 1;}
    if (absorbation.y > 1) { absorbation.y = 1;}
    if (absorbation.z > 1) { absorbation.z = 1;}

    //cast rays
    Color reflect_col = reflected_ray.Cast(scene);
    Color refract_col = refracted_ray.Cast(scene);
    Color res_col = R*reflect_col + (1-R)*refract_col; 
    return res_col*(Color(1,1,1)-absorbation);
};

void Schlick::SetConfig(const Vec3f& dir, const Vec3f& dot, const Vec3f& outer_normal, Vec3f& normal, float& n1, float& n2, float& alpha, Color& abs_coef) const {
    normal = outer_normal;
    float cos_alpha = cos(normal,dir);
    if (cos_alpha <= 0) {//ray is in extetrior
        n1 = outer_refractive_index; 
        n2 = inner_refractive_index;
        //normal - o.k;
        alpha = angle(-normal,dir);
        abs_coef = 0;
    } else {             //ray is in interior
       n1 = inner_refractive_index;
       n2 = outer_refractive_index;
       normal = -normal;
       alpha = angle(-normal,dir);
       abs_coef = GetAbsorbtionCoefficient();
    }
    //now we are in n1, hitting n2, normal - on us, alpha - acute normal angle.
};

#endif //__interMod_cpp__
