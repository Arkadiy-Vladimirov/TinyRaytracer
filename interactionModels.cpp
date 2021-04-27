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

//Color LambertEmission::Interact(const Ray& ray, const Vec3f& hit_point, const Vec3f& outer_normal, const GrObjCollection& scene) const {
//    return cos(-outer_normal,ray.GetDirection()) * emission;
//};

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
    SetConfig(r,hit_point,outer_normal,  n,n1,n2,alpha);

    //count directions(Snell's law)
    float beta = asin(n1/n2 * sin(alpha));
    Vec3f tau = r+cos(alpha)*n; if (norm(tau) != 0) { tau = normalize(tau); } 
    Vec3f a = cos(alpha)*n   + sin(alpha)*tau;
    Vec3f b = cos(beta)*(-n) + sin(beta) *tau;
    
    //count intensivities
    //Schlick's approximation
        float R0 = pow(((n1-n2)/(n1+n2)),2);
        float R = R0 + (1-R0)*pow((1-cos(alpha)),5); //intensivity of reflected ray 

    //create rays
    Ray reflected_ray(hit_point + Ray::GetEpsilon()*a, a, ray.GetRecursionDepth() + 1);
    Ray refracted_ray(hit_point + Ray::GetEpsilon()*b, b, ray.GetRecursionDepth() + 1);
    return R*reflected_ray.Cast(scene) + (1-R)*refracted_ray.Cast(scene);
};

void Schlick::SetConfig(const Vec3f& dir, const Vec3f& dot, const Vec3f& outer_normal, Vec3f& normal, float& n1, float& n2, float& alpha) const {
    normal = outer_normal;
    float cos_alpha = cos(normal,dir);
    if (cos_alpha <= 0) {//ray is in extetrior
        n1 = outer_refractive_index; 
        n2 = inner_refractive_index;
        //normal - o.k;
        alpha = angle(-normal,dir);
    } else {             //ray is in interior
       n1 = inner_refractive_index;
       n2 = outer_refractive_index;
       normal = -normal;
       alpha = angle(-normal,dir); 
    }
    //now we are in n1, hitting n2, normal - on us, alpha - acute normal angle.
};

#endif //__interMod_cpp__