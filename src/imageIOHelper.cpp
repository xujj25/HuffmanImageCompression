//
// Created by xujijun on 12/10/17.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include "imageIOHelper.h"
#include "imageIOException.h"
using namespace std;

const uint32_t ImageIOHelper::BitmapInfoLength = 54;

const uint32_t ImageIOHelper::BitmapWidthOffset = 18;
const uint32_t ImageIOHelper::BitmapHeightOffset= 22;
const uint32_t ImageIOHelper::BitmapSizeOffset = 34;

const uint32_t ImageIOHelper::BitmapPixelBitSizeOffset = 28;
const uint32_t ImageIOHelper::BitmapTrueColorBitSize = 24;
const uint32_t ImageIOHelper::BitmapTrueColorChannelCount = 3;

const uint32_t ImageIOHelper::EncodeFileInfoLength = 8;

Image* ImageIOHelper::readImage(const string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");  // open the image file in mode "read binary"
    if (fp == nullptr)
        throw ImageIOException("Failed to open file");

    auto imageInfoPtr = new unsigned char[BitmapInfoLength];
    fread(imageInfoPtr, sizeof(unsigned char), BitmapInfoLength, fp);  // read the header

    // get bit size of the image
    uint32_t bitSize = *reinterpret_cast<uint32_t *>(&imageInfoPtr[BitmapPixelBitSizeOffset]);
    if (bitSize != BitmapTrueColorBitSize) {
        fclose(fp);
        throw ImageIOException("Input file is not a true color bitmap");
    }

    // get image height and width
    uint32_t width = *reinterpret_cast<uint32_t *>(&imageInfoPtr[BitmapWidthOffset]);
    uint32_t height = *reinterpret_cast<uint32_t *>(&imageInfoPtr[BitmapHeightOffset]);
    uint32_t rawSize = *reinterpret_cast<uint32_t *>(&imageInfoPtr[BitmapSizeOffset]);
    uint32_t dataSize = 3 * width * height;  // 3 Bytes per pixel

    unsigned char* rawDataPtr = new unsigned char[rawSize + 1];
    unsigned char* dataPtr = new unsigned char[dataSize + 1];
    memset(rawDataPtr, '\0', rawSize + 1);
    memset(dataPtr, '\0', dataSize + 1);

    fread(rawDataPtr, sizeof(unsigned char), rawSize, fp);  // read the rest of the data
    fclose(fp);
    uint32_t row, col, pixelIndex, byteIndex;
    const uint32_t nullBytesCountInRow = rawSize / height - width * BitmapTrueColorChannelCount;
    uint32_t pixelOffset = 0;
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            pixelIndex = col + row * width;
            byteIndex = pixelIndex * BitmapTrueColorChannelCount;
            memcpy(&dataPtr[byteIndex], &rawDataPtr[pixelOffset], BitmapTrueColorChannelCount);
            pixelOffset += BitmapTrueColorChannelCount;
        }
        // increase the pixelOffset for the null bytes in every row in the bmp matrix
        pixelOffset += nullBytesCountInRow;
    }

    Image* imagePtr = new Image(dataPtr, imageInfoPtr, width, height, rawSize, dataSize);

    delete [] rawDataPtr;
    return imagePtr;
}

EncodeFile *ImageIOHelper::readEncodeFile(const string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");  // open the image file in mode "read binary"
    if (fp == nullptr)
        throw ImageIOException("Failed to open file");

    // read imageInfo
    auto imageInfoPtr = new unsigned char[BitmapInfoLength];
    fread(imageInfoPtr, sizeof(unsigned char), BitmapInfoLength, fp);  // read the header

    // read dataBitSize and weightMapValCount
    auto encodeFileInfoPtr = new unsigned char[EncodeFileInfoLength];
    fread(encodeFileInfoPtr, sizeof(unsigned char), EncodeFileInfoLength, fp);
    uint32_t dataBitSize = *reinterpret_cast<uint32_t *>(encodeFileInfoPtr);
    uint32_t weightMapValCount = *reinterpret_cast<uint32_t *>(encodeFileInfoPtr + EncodeFileInfoLength / 2);
    delete [] encodeFileInfoPtr;

    // read weightMapData
    const size_t weightMapDataSize = weightMapValCount * EncodeFile::WeightMapElementSize;
    auto weightMapDataPtr =
            new unsigned char[weightMapDataSize + 1];
    fread(weightMapDataPtr, sizeof(unsigned char), weightMapDataSize, fp);

    // read encode data
    uint32_t dataSize = dataBitSize / 8 + (dataBitSize % 8 == 0 ? 0 : 1);

    auto dataPtr = new unsigned char[dataSize + 1];
    memset(dataPtr, '\0', dataSize + 1);
    fread(dataPtr, sizeof(unsigned char), dataSize, fp);
    fclose(fp);

    EncodeFile* encodeFilePtr =
            new EncodeFile(imageInfoPtr, weightMapDataPtr, dataPtr, dataBitSize, weightMapValCount);

    return encodeFilePtr;
}

void ImageIOHelper::writeEncodeFile(const string& filename,
        const unsigned char* imageInfo,
        const unordered_map<unsigned char, uint32_t> weightMap,
        const unsigned char* encodeData,
        uint32_t encodeDataSize,
        uint32_t encodeDataBitSize) {
    FILE* fp = fopen(filename.c_str(), "wb");

    if (fp == nullptr)
        throw ImageIOException("Failed to open file");

    // write imageInfo
    fwrite(imageInfo, sizeof(unsigned char), BitmapInfoLength, fp);

    // write encodeDataInfo: encodeDataBitSize and weightMapValCount
    uint32_t weightMapValCount = static_cast<uint32_t>(weightMap.size());
    auto encodeFileInfoPtr = new unsigned char[EncodeFileInfoLength];

    int byteIndex;
    for (byteIndex = 0; byteIndex < 4; byteIndex++) {
        encodeFileInfoPtr[byteIndex] =
                static_cast<unsigned char>(0xff & (encodeDataBitSize >> (byteIndex * 8)));
        encodeFileInfoPtr[byteIndex + 4] =
                static_cast<unsigned char>(0xff & (weightMapValCount >> (byteIndex * 8)));
    }
    fwrite(encodeFileInfoPtr, sizeof(unsigned char), EncodeFileInfoLength, fp);

    // write weightMap
    const size_t weightMapArrSize = weightMapValCount * EncodeFile::WeightMapElementSize;
    unsigned char *weightMapArr = new unsigned char[weightMapArrSize];
    int elementIndex = 0;
    for (auto& element: weightMap) {
        weightMapArr[elementIndex * EncodeFile::WeightMapElementSize] = element.first;
        for (byteIndex = 0; byteIndex < 4; byteIndex++) {
            weightMapArr[elementIndex * EncodeFile::WeightMapElementSize + byteIndex + 1] =
                    static_cast<unsigned char>(0xff & (element.second >> (byteIndex * 8)));
        }
        elementIndex++;
    }
    fwrite(weightMapArr, sizeof(unsigned char), weightMapArrSize, fp);

    // write encode data
    fwrite(encodeData, sizeof(unsigned char), encodeDataSize, fp);

    delete [] encodeFileInfoPtr;
    delete [] weightMapArr;
    fclose(fp);
}

void ImageIOHelper::writeImage(const string &filename,
        unsigned char* imageInfoPtr,
        const unsigned char* decodedDataPtr) {

    FILE* fp = fopen(filename.c_str(), "wb");

    if (fp == nullptr)
        throw ImageIOException("Failed to open file");

    // write imageInfo
    fwrite(imageInfoPtr, sizeof(unsigned char), BitmapInfoLength, fp);

    // get image height and width
    uint32_t width = *reinterpret_cast<uint32_t *>(&imageInfoPtr[BitmapWidthOffset]);
    uint32_t height = *reinterpret_cast<uint32_t *>(&imageInfoPtr[BitmapHeightOffset]);
    uint32_t rawSize = *reinterpret_cast<uint32_t *>(&imageInfoPtr[BitmapSizeOffset]);

    const uint32_t nullBytesCountInRow = rawSize / height - width * BitmapTrueColorChannelCount;
    unsigned char *rawDataPtr = new unsigned char[rawSize];
    memset(rawDataPtr, '\0', rawSize);

    uint32_t row, col, pixelOffset = 0;
    uint32_t pixelIndex, byteIndex;
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            pixelIndex = col + row * width;
            byteIndex = pixelIndex * BitmapTrueColorChannelCount;
            // get 3 color channel value in every pixel
            memcpy(&rawDataPtr[pixelOffset], &decodedDataPtr[byteIndex], BitmapTrueColorChannelCount);
            pixelOffset += BitmapTrueColorChannelCount;
        }
        // increase the pixelOffset for the null bytes in every row in the bmp matrix
        pixelOffset += nullBytesCountInRow;
    }

    fwrite(rawDataPtr, sizeof(unsigned char), rawSize, fp);

    delete [] rawDataPtr;
    fclose(fp);
}
