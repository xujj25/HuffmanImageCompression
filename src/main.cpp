//
// Created by xujijun on 12/31/17.
//

#include <cstdio>
#include <iostream>
#include <string>
#include <imageIOException.h>
#include "huffmanCompression.h"
#include "imageIOHelper.h"
using namespace std;

void encodeImage() {
    string imageFilename, encodedFilename;
    cout << "Please input image filename(BMP format):" << endl;
    cin >> imageFilename;
    if (imageFilename.length() < 5 || imageFilename.substr(imageFilename.length() - 4) != ".bmp") {
        throw ImageIOException("image format wrong!");
    }

    cout << "Please input encoded filename(with extension name \".hfmc\"):" << endl;
    cin >> encodedFilename;
    if (encodedFilename.length() < 6 || encodedFilename.substr(encodedFilename.length() - 5) != ".hfmc") {
        throw ImageIOException("encoded filename format wrong!");
    }

    Image* imagePtr = ImageIOHelper::readImage(imageFilename);

    cout << "Width = " << imagePtr -> getWidth() << endl;
    cout << "Height = " << imagePtr -> getHeight() << endl;
    cout << "Pure dataSize = " << imagePtr -> getDataSize() << " Bytes" << endl;
    cout << "RawDataSize = " << imagePtr -> getRawDataSize() << " Bytes" << endl;

    HuffmanCompression encoder;
    unordered_map<unsigned char, uint32_t> weightMap;
    unsigned char *encodedDataPtr = nullptr;
    uint32_t encodedDataBitSize;
    encoder.getEncodedData(imagePtr -> getData(), imagePtr -> getDataSize(),
                           weightMap, encodedDataPtr, encodedDataBitSize);

    cout << "Encoded data bit size: " << encodedDataBitSize << endl;
    uint32_t encodedDataSize = encodedDataBitSize / 8 + (encodedDataBitSize % 8 == 0 ? 0 : 1);
    cout << "EncodedDataSize: " << encodedDataSize << " Bytes" << endl;

    printf("Compression ratio = %.2f%%\n",
           static_cast<double>(encodedDataSize) /
                   static_cast<double >(imagePtr -> getRawDataSize()) * 100);

    ImageIOHelper::writeEncodeFile(encodedFilename, imagePtr -> getImageInfo(), weightMap,
                                   encodedDataPtr, encodedDataSize, encodedDataBitSize);

    delete [] encodedDataPtr;
    delete imagePtr;
}

void decodeImage() {
    string imageFilename, encodedFilename;

    cout << "Please input encoded filename(with extension name \".hfmc\"):" << endl;
    cin >> encodedFilename;
    if (encodedFilename.length() < 6 || encodedFilename.substr(encodedFilename.length() - 5) != ".hfmc") {
        throw ImageIOException("encoded filename format wrong!");
    }

    cout << "Please input image filename(BMP format):" << endl;
    cin >> imageFilename;
    if (imageFilename.length() < 5 || imageFilename.substr(imageFilename.length() - 4) != ".bmp") {
        throw ImageIOException("image format wrong!");
    }

    EncodeFile* encodeFilePtr = ImageIOHelper::readEncodeFile(encodedFilename);
    HuffmanCompression decoder;

    unsigned char *decodedDataPtr = nullptr;
    uint32_t decodedDataSize;

    decoder.getDecodedData(encodeFilePtr -> getData(),
                           encodeFilePtr -> getDataBitSize(),
                           encodeFilePtr -> getWeightMapArr(),
                           decodedDataPtr, decodedDataSize);

    cout << "Decoded data size: " << decodedDataSize << " Bytes" << endl;

    ImageIOHelper::writeImage(imageFilename,
                              encodeFilePtr -> getImageInfo(),
                              decodedDataPtr);

    delete [] decodedDataPtr;
    delete encodeFilePtr;
}

int main() {
    string command;
    const string EncodeCmd = "e";
    const string DecodeCmd = "d";
    cout << "Input your choice:(\"e\"for encode, \"d\"for decode): ";
    cin >> command;
    if (command == EncodeCmd) {
        encodeImage();
    } else if (command == DecodeCmd) {
        decodeImage();
    } else {
        cout << endl;
        cout << "Wrong Command!" << endl;
    }

    return 0;
}
