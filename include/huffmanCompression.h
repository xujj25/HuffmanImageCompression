//
// Created by xujijun on 12/31/17.
//


#ifndef HUFFMANIMAGECOMPRESSION_HUFFMANCOMPRESSION_H
#define HUFFMANIMAGECOMPRESSION_HUFFMANCOMPRESSION_H

#include <cstdio>
#include <queue>
#include <vector>
#include "hfmCodeBitSet.h"
#include <unordered_map>
using namespace std;


class HuffmanCompression {
private:
    struct TreeNode {
        unsigned char val;
        uint32_t weight;
        TreeNode *left;
        TreeNode *right;
        TreeNode(unsigned char _val, uint32_t _weight) : val(_val), weight(_weight), left(nullptr), right(nullptr) {}
    };

    struct nodePtrCmp {
        bool operator() (TreeNode* a, TreeNode* b) {
            return (a -> weight) >= (b -> weight);
        }
    };

    TreeNode *treeRoot;

    priority_queue<TreeNode*, vector<TreeNode*>, nodePtrCmp> nodeQueue;

    void calcWeight(const unsigned char*, uint32_t ,unordered_map<unsigned char, uint32_t >&);

    void generateEncodedNodeQueue(const unordered_map<unsigned char, uint32_t >&);

    void generateDecodedNodeQueue(const vector<pair<unsigned char, uint32_t >>&);

    void buildTree();

    void dfs(TreeNode*, hfmCodeBitSet&, unordered_map<unsigned char, hfmCodeBitSet>&);
    void getCodeMap(unordered_map<unsigned char, hfmCodeBitSet>&);

    uint32_t calcEncodedOutputSize(const unordered_map<unsigned char, uint32_t >&,
                              const unordered_map<unsigned char, hfmCodeBitSet>&);
    void generateEncodedOutput(const unsigned char *, uint32_t ,
                               const unordered_map<unsigned char, hfmCodeBitSet> &,
                               unsigned char *&, uint32_t);
    uint32_t calcDecodedOutputSize(const vector<pair<unsigned char, uint32_t >>& );
    void generateDecodedOutput(const unsigned char *, uint32_t ,
                               unsigned char *&, uint32_t);

public:
    HuffmanCompression();
    ~HuffmanCompression();
    void getEncodedData(const unsigned char* , uint32_t ,
            unordered_map<unsigned char, uint32_t >& , unsigned char* &, uint32_t &);
    void getDecodedData(const unsigned char* , uint32_t ,
                        const vector<pair<unsigned char, uint32_t >>& , unsigned char* &, uint32_t &);
};

#endif