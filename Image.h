#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>
#include <fstream>

struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

struct Point                //
{
  int x;
  int y;
};

constexpr Pixel backgroundColor{0, 0, 0, 0};

struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);
  Image(const Image& img);

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()        { return data; }

  Pixel GetPixel(int x, int y) const { return data[width * y + x];}
  void  PutPixel(int x, int y, const Pixel &pix, bool alpha_compose = false);

  void PutImage(int x_off, int y_off, const std::string &a_path);

  Image& operator = (const Image &img);

  ~Image();

private:
  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  bool self_allocated = false;
};

#endif //MAIN_IMAGE_H
