#include <stdexcept>
#include <cstring>
#include "hfmCodeBitSet.h"
using namespace std;

const uint32_t hfmCodeBitSet::DataBlockLength = 32; // an color channel value won't be larger than 256

hfmCodeBitSet::hfmCodeBitSet() {
    len = 0;
    data = new unsigned char[DataBlockLength];
    memset(data, 0, DataBlockLength);
}

hfmCodeBitSet::hfmCodeBitSet(const hfmCodeBitSet &other) {
    *this = other;
}

uint32_t hfmCodeBitSet::length() const {
    return len;
}

bool hfmCodeBitSet::append(unsigned char flag) {
    if (len >= DataBlockLength * 8) {
        throw runtime_error("hfmCodeBitSet::append error: hfmCodeBitSet overflow");
    }
    uint32_t byteOff = len / 8;
    uint32_t bitOff = len % 8;
    if (flag & 1)
        data[byteOff] |= (1 << bitOff);
    else
        data[byteOff] &= (~(1 << bitOff));
    len++;
    return true;
}

bool hfmCodeBitSet::pop_back() {
    if (len <= 0) {
        throw runtime_error("hfmCodeBitSet::pop_back error: hfmCodeBitSet underflow");
    }
    len--;
    return true;
}

hfmCodeBitSet::~hfmCodeBitSet() {
    delete[](data);
}

bool hfmCodeBitSet::operator[](uint32_t pos) const {
    if (pos >= len) {
        throw runtime_error("hfmCodeBitSet::[] error: access overflow");
    }
    if (pos < 0) {
        throw runtime_error("hfmCodeBitSet::[] error: access underflow");
    }
    uint32_t byteOff = pos / 8;
    uint32_t bitOff = pos % 8;
    bool res = static_cast<bool>((data[byteOff] >> bitOff) & 1);
    return res;
}

unsigned char *hfmCodeBitSet::getData() const {
    return data;
}

hfmCodeBitSet& hfmCodeBitSet::operator=(const hfmCodeBitSet &other) {
    len = other.length();
    memcpy(data, other.getData(), DataBlockLength);

    return *this;
}
