#include "imageIOException.h"

//
// Created by xujijun on 12/10/17.
//

ImageIOException::ImageIOException(const string &__arg) :
        runtime_error("ImageIOException: " +  __arg) {}
