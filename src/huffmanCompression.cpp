//
// Created by xujijun on 12/31/17.
//


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

// 因为节点队列能够根据进出队列的节点的权值进行顺序调整，
// 所以每次只需要从队首取出2个节点构造一棵树，将树根再次加进节点队列，
// 直到队列只剩下一个节点，这个节点就是编码树的根节点
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

// 深度优先搜索，过程中使用path来记录路径，
// 到达每个叶子节点的path就是叶子节点对应数据的编码
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

// 生成编码表：调用dfs来生成编码表
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
    calcWeight(rawDataPtr, rawDataSize, dstWeightMap);  // 计算出权值映射表
    generateEncodedNodeQueue(dstWeightMap);  // 根据权值映射表构建编码树节点队列
    // （使用priority_queue这样一个以堆实现的“队列”，可以保证每次进出队操作后队列的节点保持权值从小到大排列）

    buildTree();  // 建立编码树
    unordered_map<unsigned char, hfmCodeBitSet> resCodeMap;
    getCodeMap(resCodeMap);  // 根据编码树产生权值表
    outputDataBitSize = calcEncodedOutputSize(dstWeightMap, resCodeMap);  // 计算编码数据位数
    generateEncodedOutput(rawDataPtr, rawDataSize, resCodeMap, outputDataPtr, outputDataBitSize);  // 对原数据进行编码
}

void HuffmanCompression::getDecodedData(const unsigned char *rawDataPtr, uint32_t rawDataBitSize,
                                        const vector<pair<unsigned char, uint32_t >> &srcWeightMapArr,
                                        unsigned char *&outputDataPtr, uint32_t &outputDataSize) {
    generateDecodedNodeQueue(srcWeightMapArr);  // 根据读取文件之后构建的权值映射表构建节点队列
    buildTree();  // 建立编码树
    outputDataSize = calcDecodedOutputSize(srcWeightMapArr);  // 计算输出数据大小
    generateDecodedOutput(rawDataPtr, rawDataBitSize, outputDataPtr, outputDataSize);  // 对编码数据进行解码
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




