//
// Created by xujijun on 12/31/17.
//

#ifndef HUFFMANIMAGECOMPRESSION_ENCODEFILE_H
#define HUFFMANIMAGECOMPRESSION_ENCODEFILE_H

#include <vector>
#include <cstdint>
using namespace std;

class EncodeFile {
    unsigned char *imageInfo;
    unsigned char *weightMapData;
    unsigned char *data;
    uint32_t dataBitSize;
    uint32_t weightMapValCount;

    vector<pair<unsigned char, uint32_t>> weightMapArr;

    void generateWeightMapArr();
public:
    EncodeFile(unsigned char* , unsigned char*, unsigned char*, uint32_t, uint32_t);
    ~EncodeFile();
    vector<pair<unsigned char, uint32_t>>& getWeightMapArr();
    uint32_t getDataBitSize();
    unsigned char* getData();
    unsigned char* getImageInfo();

    static const uint32_t WeightMapElementSize;

};


#endif //HUFFMANIMAGECOMPRESSION_ENCODEFILE_H
