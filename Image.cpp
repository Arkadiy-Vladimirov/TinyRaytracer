#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

Pixel AlphaCompose(const Pixel &new_p, const Pixel &old_p) {
  float alpha =  new_p.a/255;
  uint8_t rc = new_p.r * (alpha) + old_p.r * (1-alpha);
  uint8_t gc = new_p.g * (alpha) + old_p.g * (1-alpha);
  uint8_t bc = new_p.b * (alpha) + old_p.b * (1-alpha);
  uint8_t ac = 255;
  return Pixel {.r=rc, .g=gc, .b=bc, .a=ac};
}

Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, 0)) != nullptr)
  {
    size = width * height * channels;
  } else throw "invalid tile path"; //
  std::cout << "texture uploaded: " << a_path << ' ' <<  width << ' ' <<  height << ' '  << channels << '\n'; // 
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height ]{};

  if(data != nullptr)
  {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
  }
}

Image::Image(const Image& img) {
  width = img.width; height = img.height; channels = img.channels; size = img.size;
  data = new Pixel[width * height]{};
  for (int x=0; x<width; ++x){
  for (int y=0; y<height; ++y){
      data[width* y + x] = img.data[width* y + x];
  }
  } 
  self_allocated = true;
}

Image& Image::operator = (const Image &img) {
  if ((width == img.width) && (height == img.height) && (channels == img.channels) && (size = img.size)) {
    for (int x=0; x<width; ++x){
    for (int y=0; y<height; ++y){
      data[width* y + x] = img.data[width* y + x];
    }
    } 
    return *this;
  } else throw "assignment attempt of incompatible images"; 
} 

int Image::Save(const std::string &a_path)
{
  Pixel pix;
  for (int i = 0; i < height / 2; ++i) {
  for (int j = 0; j < width; ++j) {
    pix = data[i * width + j];
    data[i * width + j] = data[(height - i) * width + j];
    data[(height - i) * width + j] = pix;
  }
  }
  auto extPos = a_path.find_last_of('.');
  if(a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
  }
  else if(a_path.substr(extPos, std::string::npos) == ".jpg" || a_path.substr(extPos, std::string::npos) == ".JPG" ||
          a_path.substr(extPos, std::string::npos) == ".jpeg" || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  }
  else
  {
    std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path << "\n";
    return 1;
  }

  return 0;
}

void Image::PutPixel(int x, int y, const Pixel &pix, bool alpha_compose) {
  if (!alpha_compose)
    data[width* y + x] = pix;
  else {
    Pixel old_pix = GetPixel(x,y);
    data[width* y + x] = AlphaCompose(pix,old_pix);
  }

}

void Image::PutImage(int x_off, int y_off, const std::string &a_path){
  Image PI(a_path);
  for (int x = 0; x < PI.width ; ++x) {
  for (int y = 0; y < PI.height; ++y) {
     PutPixel(x_off + x, y_off + y, PI.GetPixel(x,PI.height-y-1),true);
  }
  }
}

Image::~Image()
{
  if(self_allocated)
    delete [] data;
  else
  {
    stbi_image_free(data);
  }
}