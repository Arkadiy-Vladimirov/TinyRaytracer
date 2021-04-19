#ifndef __grObj_cpp__
#define __grObj_cpp__

#include "graphicObjects.hpp"

float Camera::render_distance = 1000;

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


Scene::~Scene() {
    for (unsigned i = 0; i < size; ++i)
        delete data[i];
    delete[] data;
};

GraphObject*& Scene::operator[](unsigned idx) const{
    if (idx < size)
        return data[idx];
    else throw "error: array index out of bounds";
};


Vec3f MonochromeSphere::CheckHit(const Ray& ray) const {
    return Vec3f();
};

Color MonochromeSphere::Hit(const Ray& ray) const {
    return Color();
};

Color Ray::Cast(const Scene& scene) const {
    Vec3f old_hit_point = Camera::GetRenderDistance() * direction; //inaccurate
    Color resCol; int hit_obj_idx = -1;
    Vec3f new_hit_point;

    for (int i = 0; i < scene.GetSize(); ++i) {
        new_hit_point = scene[i]->CheckHit(*this);
        if (norm(new_hit_point) < norm(old_hit_point)) {
            old_hit_point = new_hit_point;
            hit_obj_idx = i;
        };
    };

    if (hit_obj_idx != -1)
        resCol = scene[hit_obj_idx]->Hit(*this);
    
    return resCol;
};

#endif //__grObj_cpp__
