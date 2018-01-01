//
// Created by xujijun on 12/10/17.
//

#ifndef HUFFMANIMAGECOMPRESSION_IMAGEIOEXCEPTION_H
#define HUFFMANIMAGECOMPRESSION_IMAGEIOEXCEPTION_H

#include <stdexcept>
#include <string>
using namespace std;

class ImageIOException: public runtime_error {
public:
    explicit ImageIOException(const string &__arg);
};

#endif //HUFFMANIMAGECOMPRESSION_IMAGEIOEXCEPTION_H
