#include <iostream>
#include <cstring>
#include "huffmanCompression.h"
using namespace std;

HuffmanCompression::HuffmanCompression() {
    treeRoot = nullptr;
}

HuffmanCompression::~HuffmanCompression() {
    if (treeRoot == nullptr)
        return;
    queue<TreeNode*> q;
    q.push(treeRoot);
    while (!q.empty()) {
        TreeNode* node  = q.front();
        q.pop();
        if (node -> left != nullptr)
            q.push(node -> left);
        if (node -> right != nullptr)
            q.push(node -> right);
        delete(node);
    }
    treeRoot = nullptr;
}

void HuffmanCompression::calcWeight(const unsigned char* dataPtr, uint32_t dataSize,
                                    unordered_map<unsigned char, uint32_t >& weightMap) {
    for (uint32_t i = 0; i < dataSize; i++) {
        if (weightMap.find(dataPtr[i]) == weightMap.end()) {
            weightMap[dataPtr[i]] = 1;
        } else {
            weightMap[dataPtr[i]]++;
        }
    }
}

void HuffmanCompression::buildTree() {
    while (nodeQueue.size() > 1) {
        TreeNode* rightNode = nodeQueue.top();
        nodeQueue.pop();
        TreeNode* leftNode = nodeQueue.top();
        nodeQueue.pop();
        auto parentNode = new TreeNode(0, rightNode -> weight + leftNode -> weight);
        parentNode -> left = leftNode;
        parentNode -> right = rightNode;
        nodeQueue.push(parentNode);
    }
    treeRoot = nodeQueue.top();
}

void HuffmanCompression::dfs(TreeNode* node, hfmCodeBitSet& path,
                             unordered_map<unsigned char, hfmCodeBitSet> &resCodeMap) {
    if (node -> left == nullptr && node -> right == nullptr) {
        resCodeMap[node -> val] = path;
        return;
    }
    path.append(0);
    dfs(node -> left, path, resCodeMap);
    path.pop_back();
    path.append(1);
    dfs(node -> right, path, resCodeMap);
    path.pop_back();
}

void HuffmanCompression::getCodeMap(unordered_map<unsigned char, hfmCodeBitSet> &resCodeMap) {
    if (treeRoot == nullptr)
        return;
    hfmCodeBitSet path;
    dfs(treeRoot, path, resCodeMap);
}

void HuffmanCompression::generateEncodedOutput(const unsigned char *rawDataPtr, uint32_t rawDataSize,
                                               const unordered_map<unsigned char, hfmCodeBitSet> &resCodeMap,
                                               unsigned char* &outputDataPtr, uint32_t outputDataBitSize) {
    outputDataPtr = new unsigned char[outputDataBitSize / 8 + 1];
    memset(outputDataPtr, 0, outputDataBitSize / 8 + 1);
    uint32_t outputBitOffset = 0;
    uint32_t codeBitLength = 0;
    uint32_t byteOff, bitOffInByte;
    uint32_t i, j;

    // generate encode output data by looking up the weight map
    for (i = 0; i < rawDataSize; i++) {
        const hfmCodeBitSet& code = resCodeMap.at(rawDataPtr[i]);
        codeBitLength = code.length();
        for (j = 0; j < codeBitLength; j++) {
            byteOff = (j + outputBitOffset) / 8;
            bitOffInByte = (j + outputBitOffset) % 8;
            outputDataPtr[byteOff] |= (code[j] << bitOffInByte);
        }
        outputBitOffset += codeBitLength;
    }
    if (outputBitOffset != outputDataBitSize) {
        throw runtime_error("HuffmanCompression::generateEncodedOutput error: bitCount not match");
    }
}

void HuffmanCompression::generateDecodedOutput(const unsigned char *rawDataPtr, uint32_t rawDataBitSize,
                                               unsigned char *&outputDataPtr, uint32_t outputDataSize) {
    outputDataPtr = new unsigned char[outputDataSize + 1];
    memset(outputDataPtr, 0, outputDataSize + 1);
    uint32_t bitOffInByte = 0;
    uint32_t byteOff = 0;
    uint32_t rawDataBitOff;
    uint32_t outputDataOff = 0;
    TreeNode *curNode = treeRoot;

    // generate decode output data by DFS the Huffman Tree
    for (rawDataBitOff = 0; rawDataBitOff < rawDataBitSize; rawDataBitOff++) {
        if (curNode -> left == nullptr && curNode -> right == nullptr) {
            outputDataPtr[outputDataOff++] = curNode -> val;
            curNode = treeRoot;
        }
        byteOff = rawDataBitOff / 8;
        bitOffInByte = rawDataBitOff % 8;
        if ((rawDataPtr[byteOff] >> bitOffInByte) & 1) {
            if (curNode == nullptr || curNode -> right == nullptr) {
                throw runtime_error("HuffmanCompression::generateDecodedOutput error: bit not match");
            } else {
                curNode = curNode -> right;
            }
        } else {
            if (curNode == nullptr || curNode -> left == nullptr) {
                throw runtime_error("HuffmanCompression::generateDecodedOutput error: bit not match");
            } else {
                curNode = curNode -> left;
            }
        }
    }
    if (curNode -> left == nullptr && curNode -> right == nullptr) {
        outputDataPtr[outputDataOff++] = curNode -> val;
        curNode = treeRoot;
    }
    if (outputDataOff != outputDataSize) {
        throw runtime_error("HuffmanCompression::generateDecodedOutput error: outputData overflow");
    }
}

void HuffmanCompression::getEncodedData(const unsigned char *rawDataPtr, uint32_t rawDataSize,
                                        unordered_map<unsigned char, uint32_t > &dstWeightMap,
                                        unsigned char *&outputDataPtr, uint32_t &outputDataBitSize) {
    calcWeight(rawDataPtr, rawDataSize, dstWeightMap);
    generateEncodedNodeQueue(dstWeightMap);
    buildTree();
    unordered_map<unsigned char, hfmCodeBitSet> resCodeMap;
    getCodeMap(resCodeMap);
    outputDataBitSize = calcEncodedOutputSize(dstWeightMap, resCodeMap);
    generateEncodedOutput(rawDataPtr, rawDataSize, resCodeMap, outputDataPtr, outputDataBitSize);
}

void HuffmanCompression::getDecodedData(const unsigned char *rawDataPtr, uint32_t rawDataBitSize,
                                        const vector<pair<unsigned char, uint32_t >> &srcWeightMapArr,
                                        unsigned char *&outputDataPtr, uint32_t &outputDataSize) {
    generateDecodedNodeQueue(srcWeightMapArr);
    buildTree();
    outputDataSize = calcDecodedOutputSize(srcWeightMapArr);
    generateDecodedOutput(rawDataPtr, rawDataBitSize, outputDataPtr, outputDataSize);
}

uint32_t HuffmanCompression::calcEncodedOutputSize(const unordered_map<unsigned char, uint32_t > &weightMap,
                                              const unordered_map<unsigned char, hfmCodeBitSet> &resCodeMap) {
    uint32_t bitsCount = 0;
    unsigned char val;
    uint32_t weight, codeLength;

    for (const auto &ele : weightMap) {
        val = ele.first;
        weight = ele.second;
        codeLength = static_cast<uint32_t >(resCodeMap.at(val).length());
        bitsCount += weight * codeLength;
    }
    return bitsCount;
}

uint32_t HuffmanCompression::calcDecodedOutputSize(const vector<pair<unsigned char, uint32_t >> &weightMap) {
    uint32_t res = 0;
    for (const auto &ele : weightMap) {
        res += ele.second;
    }
    return res;
}

void HuffmanCompression::generateEncodedNodeQueue(const unordered_map<unsigned char, uint32_t > &weightMap) {
    for (const auto &ele : weightMap) {
        nodeQueue.push(new TreeNode(ele.first, ele.second));
    }
}

void HuffmanCompression::generateDecodedNodeQueue(const vector<pair<unsigned char, uint32_t >> &weightMapArr) {
    for (const auto &ele : weightMapArr) {
        nodeQueue.push(new TreeNode(ele.first, ele.second));
    }
}




