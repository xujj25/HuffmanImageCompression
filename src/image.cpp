#include <cstdio>
#include <cstdint>
#include <stdint-gcc.h>
#include "image.h"

//

//
// Created by xujijun on 12/11/17.


Image::Image(unsigned char *data, unsigned char *info,
             uint32_t width, uint32_t height, uint32_t rawSize, uint32_t dataSize) {
    this -> data = data;
    this -> imageInfo = info;
    this -> width = width;
    this -> height = height;
    this -> rawDataSize = rawSize;
    this -> dataSize = dataSize;
}

Image::~Image() {
    delete [] data;
    delete [] imageInfo;
}

unsigned int Image::getRawDataSize() {
    return rawDataSize;
}

uint32_t Image::getHeight() {
    return height;
}

uint32_t Image::getWidth() {
    return width;
}

uint32_t Image::getDataSize() {
    return dataSize;
}

unsigned char *Image::getData() {
    return data;
}

unsigned char *Image::getImageInfo() {
    return imageInfo;
}


