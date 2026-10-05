#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "load_textures.h"

unsigned char* getTexturePixels(const char* address,int* width,int* height,int* channels){
  unsigned char* pixels = stbi_load(address,width,height,channels,STBI_rgb_alpha);
  return pixels;
}

void freeTexturePixels(unsigned char* pixels){
  stbi_image_free(pixels);
}
