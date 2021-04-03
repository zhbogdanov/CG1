#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>
#include <iostream>

constexpr int tileSize = 32;

struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

constexpr Pixel backgroundColor{0, 0, 0, 0};



struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);
  Image(const Image &img) : width(img.width), height(img.height), channels(img.channels), size(img.size),
                            data(img.data), self_allocated(img.self_allocated) {};

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()        { return  data; }

  Pixel GetPixel(int x, int y) { return data[height * y + x];}
  void  PutPixel(int x, int y, const Pixel &pix) { data[height * y + x] = pix; }

  Image& operator=(const Image& old) 
  {
    if (this == &old) 
    {
        return *this;
    }
    width = old.width;
    height = old.height;
    channels = old.channels;
    size = old.size;
    self_allocated = old.self_allocated;
    for (int i = 0; i < width*height; ++i)
      data[i] = old.data[i];
    return *this;
  }

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
