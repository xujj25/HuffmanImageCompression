//
// Created by xujijun on 12/11/17.
//

#ifndef HUFFMANIMAGECOMPRESSION_IMAGE_H
#define HUFFMANIMAGECOMPRESSION_IMAGE_H

class Image {
    unsigned char* imageInfo;
    unsigned char* data;
    uint32_t width;
    uint32_t height;
    uint32_t rawDataSize;
    uint32_t dataSize;
public:
    Image(unsigned char* , unsigned char* , uint32_t , uint32_t , uint32_t, uint32_t);
    ~Image();

    uint32_t getRawDataSize();
    uint32_t getHeight();
    uint32_t getWidth();
    uint32_t getDataSize();
    unsigned char* getData();
    unsigned char* getImageInfo();
};


#endif //HUFFMANIMAGECOMPRESSION_IMAGE_H
