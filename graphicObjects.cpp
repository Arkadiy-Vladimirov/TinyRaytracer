#ifndef __grObj_cpp__
#define __grObj_cpp__

#include "graphicObjects.hpp"
#include <cmath>
#include <stdio.h>


float Camera::render_distance = 1000;
float Ray::epsilon = 0.01; 
unsigned Ray::max_recursion_depth = 9;
unsigned DiffuseBall::dispersed_rays_number = 5;

//_______________Camera_Object_methods_____________________
Camera::Camera(Vec3f f_origin, Vec3f view_dir, double f_fov, Vec2un resolution, int a_channels, Vec2f f_mat_size) : cam_base(f_origin, view_dir), matrix(resolution.x, resolution.y, a_channels)  { //warning: channels!
    fov = f_fov;
    matrix_size = f_mat_size;

    //calculate matrix_origin, pixel_size, x_base and y_base.
    mat_base.orig = cam_base.orig + 1/tan(fov/2)*0.5*matrix_size.x * cam_base.e1 + 0.5*matrix_size.x * cam_base.e2 + 0.5*matrix_size.y * cam_base.e3;
    pixel_size.x = matrix_size.x/resolution.x;
    pixel_size.y = matrix_size.y/resolution.y;

    mat_base.e1 = pixel_size.x * (-cam_base.e2);
    mat_base.e2 = pixel_size.y * (-cam_base.e3); 
};

const Image& Camera::RenderImage(const Scene& scene) {
    for (int x = 0; x < matrix.Width(); ++x) {
    for (int y = 0; y < matrix.Height(); ++y) {
        Ray ray(cam_base.orig, GetPixelLocation(x,y) - cam_base.orig);
        Color col = ray.Cast(scene);
        //matrix.PutPixel(x, y, Pixel{col.r, col.g ,col.b, 255});
        //if ((x==512) && (y == 512)) {
        //    x = 512; y = 512;
        //}
        matrix.PutPixel(x, y, Pixel{static_cast<uint8_t>(round(col.x)), static_cast<uint8_t>(round(col.y)), static_cast<uint8_t>(round(col.z)), 255});
    }
    }
    return matrix;
};
//________________________________________________________


//______________Scene_object_methods______________________
Scene::~Scene() {
    for (unsigned i = 0; i < size; ++i) {
        if (data[i] != NULL)
            delete data[i];
    };
    delete[] data;
};

const GraphObject* Scene::operator[](unsigned idx) const {//to acces existing object
    if (idx < size)
        return data[idx];
    else throw "error: array index out of bounds";
};

GraphObject*& Scene::operator[](unsigned idx) {           //to create new object
    if (idx < size) {
        throw "error: non constant reference to scene object pointer"; }
    if (idx > size) {
        throw "error: non sequential data array initialization"; }
    //if (idx == size) {
        unsigned new_size = idx+1;
        GraphObject** new_data = new GraphObject*[new_size];
        for (int i = 0; i < size; ++i)
            new_data[i] = data[i];
        for (int i = size; i < new_size; ++i)
            new_data[i] = NULL;
        delete[] data;
        data = new_data;
        size = new_size;
        return data[idx];
    //};
};
//_______________________________________________________________________


//____________________Ray_Object_methods_______________________________
Color Ray::Cast(const Scene& scene) const {
    Color resCol; //default color (ray reached maximum render distance, or maximum recursion level)
    if (recursion_depth <= max_recursion_depth) {
        Vec3f hit_point;
        const GraphObject* hitted_obj_ptr = HittedObjectPtr(scene, hit_point);
        if (hitted_obj_ptr != NULL) {
            resCol = hitted_obj_ptr->Hit(*this, hit_point, scene); 
        };
    };
    return resCol;
};

const GraphObject* Ray::HittedObjectPtr(const Scene& scene, Vec3f& hit_point) const {//probably has to substitute render distance with skySphere object hit
              hit_point = orig + Camera::GetRenderDistance() * direction; //inaccurate
    Vec3f sus_hit_point;
    const GraphObject* obj_ptr = NULL; 

    for (int i = 0; i < scene.GetSize(); ++i) {
        if (scene[i]->CheckHit(*this, sus_hit_point)) { 
            if (norm(sus_hit_point - orig) < norm(hit_point - orig)) {
                hit_point = sus_hit_point;
                obj_ptr = scene[i];
            };
        };
    };

    return obj_ptr;
};
//______________________________________________________________________


//__________________________Balls'_methods______________________________
bool Ball::CheckHit(const Ray& ray, Vec3f& hit_point) const {
    //solve quadratic equation
    Vec3f a = ray.GetOrigin(), b = GetLocation().orig, l = ray.GetDirection();
    float r = GetRadius();

    float B = 2*scalar(a-b,l), C = pow(norm(a-b),2) - pow(r,2);
    float discriminant = pow(B,2) - 4*C; //A == 1
    float k;

    if (discriminant < 0) {//no intersection
        return false;
    }

    k = (-B -sqrt(discriminant)) / 2;
    if (k >= 0) {
        hit_point = a + k*l;
        return true;
    }

    k = (-B +sqrt(discriminant)) / 2;
    if (k >= 0) {
        hit_point = a + k*l;
        return true;
    }
    return false; //interection behind the ray
};

Color RefractiveBall::Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const {
    //determine where are we
    float n1, n2; //refractive indices 1 - before surface, 2 - after surface.
    float alpha;  //normal angle in rad < pi/2.
    Vec3f n;      //surface normal, direction: 2->1
    Vec3f r = ray.GetDirection();
    SetConfig(r,hit_point,n,n1,n2,alpha);

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

void RefractiveBall::SetConfig(const Vec3f& dir, const Vec3f& dot, Vec3f& normal, float& n1, float& n2, float& alpha) const {
    normal = GetNormal(dot);
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

Color DiffuseBall::Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const {
    Vec3f n = GetNormal(hit_point), r = ray.GetDirection();
    float alpha = angle(-n,r);
    Vec3f tau = r + cos(alpha) * n;
    Repere tan_bas(hit_point,n,tau,true); //e1 - normal vector, e2 - tangent vector, e3 - cross product

    Color resCol(0,0,0); //black(no light)
    float R =  1/GetDispersedRaysNumber(); //intensivity of single refracted ray
    Color RS = GetReflectanceSpectrum();

    for (int i = 0; i < GetDispersedRaysNumber(); ++i) {
        Ray reflected_ray(tan_bas.orig, GetReflectionDirection(tan_bas, alpha), ray.GetRecursionDepth() - 1);
        resCol = resCol + R*RS*reflected_ray.Cast(scene);
    }

    return resCol;
};

Vec3f DiffuseBall::GetReflectionDirection(const Repere& local_basis, float incident_angle) const {
    float phi =   float(std::rand()); phi   = phi   / RAND_MAX * PI;   // [0,pi], E(phi) = pi/2 - alpha 
    float theta = float(std::rand()); theta = theta / RAND_MAX * PI;   // [0,pi], E(thetha) = pi/2

    return sin(theta)*sin(phi)*local_basis.e1 + sin(theta)*cos(phi)*local_basis.e2 + cos(theta)*local_basis.e3; //polar transform (e1 and e2 swapped comparing to standart)
}; 

Color EmittingBall::Hit(const Ray& ray, const Vec3f& hit_point, const Scene& scene) const {
    return emission;
};
//_____________________________________________________________________

#endif //__grObj_cpp__
