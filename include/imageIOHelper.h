//
// Created by xujijun on 12/10/17.
//

#ifndef HUFFMANIMAGECOMPRESSION_IMAGEIOHELPER_H
#define HUFFMANIMAGECOMPRESSION_IMAGEIOHELPER_H

#include <string>
#include "image.h"
#include "encodeFile.h"
#include <unordered_map>

using namespace std;

class ImageIOHelper {
public:
    static const uint32_t BitmapInfoLength;

    static const uint32_t BitmapWidthOffset;
    static const uint32_t BitmapHeightOffset;
    static const uint32_t BitmapSizeOffset;

    static const uint32_t BitmapPixelBitSizeOffset;
    static const uint32_t BitmapTrueColorBitSize;
    static const uint32_t BitmapTrueColorChannelCount;

    static const uint32_t EncodeFileInfoLength;

    static Image* readImage(const string&);
    static EncodeFile* readEncodeFile(const string&);

    static void writeEncodeFile(const string& ,
                           const unsigned char* ,
                           const unordered_map<unsigned char, uint32_t> ,
                           const unsigned char* ,
                           uint32_t, uint32_t);
    static void writeImage(const string &,
                           unsigned char* ,
                           const unsigned char*);
};

#endif //HUFFMANIMAGECOMPRESSION_IMAGEIOHELPER_H
