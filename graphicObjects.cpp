#ifndef __grObj_cpp__
#define __grObj_cpp__

#include "graphicObjects.hpp"
#include <cmath>

float Camera::render_distance = 1000;

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
        matrix.PutPixel(x, y, Pixel{col.r, col.g ,col.b, 255});
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
    if (idx < size)
        throw "error: non constant reference to scene object pointer";
    if (idx > size)
        throw "error: non sequential data array initialization";
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
    Color resCol; //default color (ray reached maximum render distance)
    const GraphObject* hitted_obj_ptr = HittedObjectPtr(scene);
    if (hitted_obj_ptr != NULL)
        resCol = hitted_obj_ptr->Hit(*this);
    
    return resCol;
};

const GraphObject* Ray::HittedObjectPtr(const Scene& scene) const {//probably has to substitute render distance with skySphere object hit
    Vec3f old_hit_point = orig + Camera::GetRenderDistance() * direction; //inaccurate
    Vec3f new_hit_point;
    const GraphObject* obj_ptr = NULL; 

    for (int i = 0; i < scene.GetSize(); ++i) {
        if (scene[i]->CheckHit(*this, new_hit_point)) { 
            if (norm(new_hit_point - orig) < norm(old_hit_point - orig)) {
                old_hit_point = new_hit_point;
                obj_ptr = scene[i];
            };
        };
    };

    return obj_ptr;
};
//______________________________________________________________________


//_________________MonochromeSphere_methods______________________________
bool MonochromeSphere::CheckHit(const Ray& ray, Vec3f& hit_point) const {
    //solve quadratic equation
    Vec3f a = ray.GetOrigin(), b = location.orig, l = ray.GetDirection();
    float B = 2*scalar(a-b,l), C = pow(norm(a-b),2) - pow(radius,2);
    float discriminant = pow(B,2) - 4*C; //A == 1
    float k;

    if (discriminant < 0)//no intersection
        return false;
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

Color MonochromeSphere::Hit(const Ray& ray) const {
    return color;
};
//_____________________________________________________________________

#endif //__grObj_cpp__
