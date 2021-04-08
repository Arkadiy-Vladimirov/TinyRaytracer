#include "graphicObjects.hpp"
#include "Image.h"

Camera::Camera(Vec3 foc_pos, Vec3 v_dir, Vec2 c_fov, Vec2 resolution, int a_channels) : matrix(resolution.x, resolution.y, a_channels){
    focus = foc_pos;
    view_direction = v_dir;
    fov = c_fov;
};
