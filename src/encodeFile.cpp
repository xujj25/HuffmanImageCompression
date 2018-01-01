//
// Created by xujijun on 12/31/17.
//

#include "encodeFile.h"

const uint32_t EncodeFile::WeightMapElementSize = 5;  // every element contains 1 + 4 = 5 bytes(1 for val, 4 for weight)

EncodeFile::EncodeFile(unsigned char *imageInfo,
                       unsigned char *weightMapData,
                       unsigned char *data,
                       uint32_t dataBitSize, uint32_t weightMapValCount) {
    this -> imageInfo = imageInfo;
    this -> weightMapData = weightMapData;
    this -> data = data;
    this -> dataBitSize = dataBitSize;
    this -> weightMapValCount = weightMapValCount;
    this -> weightMapArr = vector<pair<unsigned char, uint32_t>>(weightMapValCount);
    generateWeightMapArr();
}

EncodeFile::~EncodeFile() {
    delete [] imageInfo;
    delete [] weightMapData;
    delete [] data;
}

void EncodeFile::generateWeightMapArr() {
    uint32_t valIndex;
    for (valIndex = 0; valIndex < weightMapValCount; valIndex++) {
        weightMapArr[valIndex].first = weightMapData[valIndex * WeightMapElementSize];  // set value
        weightMapArr[valIndex].second =
                *reinterpret_cast<uint32_t *>(weightMapData + (valIndex * WeightMapElementSize + 1));  // set weight
    }
}

vector<pair<unsigned char, uint32_t>>& EncodeFile::getWeightMapArr() {
    return weightMapArr;
}

uint32_t EncodeFile::getDataBitSize() {
    return dataBitSize;
}

unsigned char *EncodeFile::getData() {
    return data;
}

unsigned char *EncodeFile::getImageInfo() {
    return imageInfo;
}
