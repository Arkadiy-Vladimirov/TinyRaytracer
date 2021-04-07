#include "graphicObjects.hpp"
#include "Image.h"

Camera::Camera(int resX, int resY, int a_channels, Point3 foc_pos) : matrix(resX, resY, a_channels){
    focus = foc_pos;
};
