#include "graphicObjects.hpp"

 Camera(Vec3f f_focus, Vec3f[3] f_orient, double f_fov, Vec2un resolution,
                            int a_channels, Vec2f f_mat_size) : matrix(resolutoin.x, resolution.y, a_channels) { //warning: channels!
    focus = f_focus;
    orientation[0] = f_orient[0]; orientation[1] = f_orient[1]; orientation[2] = f_orient[2]; //+normalize
    fov = f_fov;
    matrix_size = f_mat_size;

    //calculate matrix_origin, pixel_size, x_base and y_base.
    matrix_origin = focus + orientation[0]*ctg(fov/2)*0.5*matrix_size.x + -orientation[2]*matrix_size.x*0.5 + orientaion[1]*matrix_size.y*0.5;
    pixel_size.x = matrix_size.x/resolution.x;
    pixel_size.y = matrix_size.y/resolution.y;

    x_base = pixel_size.x * orientation[2];
    y_base = pixel_size.y * orientation[1];
};
