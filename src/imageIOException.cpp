//
// Created by xujijun on 12/10/17.
//

#include "imageIOException.h"


ImageIOException::ImageIOException(const string &__arg) :
        runtime_error("ImageIOException: " +  __arg) {}
