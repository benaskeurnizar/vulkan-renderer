#pragma once


unsigned char* getTexturePixels(const char* address,int* width,int* height,int* channels);
void freeTexturePixels(unsigned char* pixels);
