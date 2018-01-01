#ifndef HUFFMANIMAGECOMPRESSION_HFMCODEBITSET_H
#define HUFFMANIMAGECOMPRESSION_HFMCODEBITSET_H

#include <string>
using namespace std;

// Huffman encode value element
class hfmCodeBitSet {
private:
    uint32_t len;
    unsigned char *data;
public:
    hfmCodeBitSet();
    explicit hfmCodeBitSet(const hfmCodeBitSet&);

    ~hfmCodeBitSet();

    uint32_t length() const ;

    bool append(unsigned char);

    bool pop_back();

    bool operator[] (uint32_t) const;

    hfmCodeBitSet& operator=(const hfmCodeBitSet&);

    unsigned char* getData() const;

    static const uint32_t DataBlockLength;
};

#endif  //HFMCODEBITSET_H
