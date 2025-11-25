//
// Created by 16374 on 2025/3/5.
//

#ifndef BICLIQUEINDEX1_BICLIQUEINDEX_H
#define BICLIQUEINDEX1_BICLIQUEINDEX_H

#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <list>
#include <vector>

// 树节点结构
struct node {
    bool is_leaf;
    int vertex;
    struct node** childs;
    int child_count;
};
typedef struct node Node;

// 新数据结构：存储需要操作的子树根节点和对应路径
struct SubtreeResult {
    Node *root;           // 子树根节点（虚拟根的直接子节点）
    std::vector<Node*> path;   // 完整路径
};

Node* create_node(int vertex, bool is_leaf);
void add_child(Node* parent, Node* child);
void print_tree(Node* node, int depth);
void free_tree(Node* root);
int addWeightForBiclique(Node* node, std::vector<float> &vertexWeight);
size_t calculateTreeMemory(Node* node, size_t* total_nodes);
void analyzeMemory(Node** roots, int rootCount);
void insertIntoPFTree(Node* root, const std::vector<int>& partialClique);
void insertIntoPrefixTree(Node* root, const std::vector<int>& sortedClique);
size_t calculatePrefixTreeMemory(Node* currentNode, size_t* totalNodes);
void analyzePrefixTreeMemory(Node* root);
void freePrefixTree(Node* root);
void merge_subtrees(Node* parent, Node* existing_child, Node* new_child);
void convert_to_prefix_tree(Node* root);
std::vector<int> intersect(const std::vector<Node*>& a, const std::vector<Node*>& b);
std::vector<std::vector<int>> computeAllIntersections(const std::vector<SubtreeResult>& data1, const std::vector<SubtreeResult>& data2);
bool isSubsequence(const std::vector<int>& s, const std::vector<int>& t);
std::vector<std::vector<int>> filterNonContained(const std::vector<std::vector<int>>& all_intersections);
void collectSubtreePaths(Node* node, int target, Node *subtreeRoot, std::vector<Node*>& currentPath,
                         std::vector<SubtreeResult>& result, bool& targetFound);
std::pair<std::vector<SubtreeResult>, std::vector<SubtreeResult>>
findVertexSubtreePaths(Node* root, int v1, int v2);
bool pathContainedInNewClique(const std::vector<Node*>& path, const std::vector<int>& newClique);
void removeRedundantSuffix(SubtreeResult& subtreeRes, const std::vector<int>& newClique);
void updatePrefixTrees_forAdd(std::vector<SubtreeResult>& paths1,
                              std::vector<SubtreeResult>& paths2,
                              const std::vector<std::vector<int>>& result, Node* prefixTreeRoot);
void addEdgeMaintain(Node *prefixTreeRoot);
void deleteTree_collect(Node* node,Node* subtreeRoot,int v0,int v1,bool has0,bool has1,std::vector<Node*>& currentPath,std::vector<SubtreeResult>& bothOut,std::vector<SubtreeResult>& eitherOut);
void findBranchesForDelete(Node* prefixTreeRoot, int v0, int v1, std::vector<SubtreeResult>& bothOut, std::vector<SubtreeResult>& eitherOut);
void removeChild_delete(Node* parent, Node* child);
std::vector<std::vector<int>> generateAugmentedCliques_delete(const std::vector<SubtreeResult>& bothOut, int v0, int v1);
void prunePaths_delete(std::vector<SubtreeResult>& bothOut, Node* prefixTreeRoot);
bool isSubsequence_delete(const std::vector<int>& seq, const std::vector<int>& subseq);
void insertResultIntoEither_delete(const std::vector<std::vector<int>>& result, const std::vector<SubtreeResult>& eitherOut, Node* prefixTreeRoot);
std::vector<std::pair<int, int>> readPairsFromFile(const std::string& filePath);
void deleteEdgeMaintain(Node *prefixTreeRoot);
void removeSubtree(Node* virtualRoot, Node* subtreeRoot);
void updateTrees_forAdd(std::vector<SubtreeResult>& paths1,
                        std::vector<SubtreeResult>& paths2,
                        const std::vector<std::vector<int>>& result, Node* prefixTreeRoot);
void addEdgeMaintain_Tree(Node *prefixTreeRoot);
void insertResultIntoEither_TreeDelete(const std::vector<std::vector<int>>& result, const std::vector<SubtreeResult>& eitherOut, Node* prefixTreeRoot);
void deleteEdgeMaintain_Tree(Node *prefixTreeRoot);
#endif //BICLIQUEINDEX1_BICLIQUEINDEX_H
