//
// Created by 16374 on 2025/3/5.
//
#include <cstdio>
#include <list>
#include <cstring>
#include <set>
#include <algorithm>
#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include "bicliqueIndex.h"
// 创建新节点
Node* create_node(int vertex, bool is_leaf) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->is_leaf = is_leaf;
    newNode->vertex = vertex;
    newNode->childs = NULL;
    newNode->child_count = 0;
    return newNode;
}

// 添加子节点
void add_child(Node* parent, Node* child) {
    parent->child_count++;
    parent->childs = (Node**)realloc(parent->childs, sizeof(Node*) * parent->child_count);
    parent->childs[parent->child_count - 1] = child;
}

// 打印树结构
void print_tree(Node* node, int depth) {
    if(!node) return;

    for(int i=0; i<depth; i++) printf("  ");
    printf("%d%s\n", node->vertex, node->is_leaf ? " (leaf)" : "");

    for(int i=0; i<node->child_count; i++) {
        print_tree(node->childs[i], depth+1);
    }
}

//求每棵树的二分团的权重之和
int addWeightForBiclique(Node* node, std::vector<float> &vertexWeight) {
    if (!node) return 0;
    float currentWeight = vertexWeight[node->vertex];
    for(int i = 0; i < node->child_count; i++) {
        currentWeight += addWeightForBiclique(node->childs[i], vertexWeight);
    }
    return currentWeight;
}

// 递归计算内存并更新统计变量
size_t calculateTreeMemory(Node* node, size_t* total_nodes) {
    if (!node) return 0;

    // 1. 计算当前节点内存
    size_t node_memory = sizeof(Node) + node->child_count * sizeof(Node*);

    // 2. 更新统计值
    (*total_nodes)++;

    // 3. 递归子节点并累加
    for (int i=0; i<node->child_count; ++i) {
        node_memory += calculateTreeMemory(node->childs[i], total_nodes);
    }

    return node_memory;
}

// 统计树索引占用内存
void analyzeMemory(Node** roots, int rootCount) {
    size_t total_mem = 0;
    size_t total_nodes = 0;

    for (int i=0; i<rootCount; ++i) {
        total_mem += calculateTreeMemory(roots[i], &total_nodes);
    }

    printf("biclique index total memory: %.4f MB\n number of nodes: %zu\n",
           total_mem/(1024.0*1024.0), total_nodes);
}

// 树的销毁函数
void free_tree(Node* root) {
    if(!root) return;
    for(int i=0; i<root->child_count; i++) {
        free_tree(root->childs[i]);
    }
    free(root->childs);
    free(root);
}

// 插入排序后的极大团到前缀树
/*void insertIntoPrefixTree(Node* root, const std::vector<int>& sortedClique) {
    Node* current = root;
    Node *p = root;//记录上一个节点
    for (int vertex : sortedClique) {
        bool found = false;
        // 检查当前节点的子节点中是否存在该顶点
        for (int i = 0; i < current->child_count; ++i) {
            //if (current->childs[i]->vertex == vertex) {
            if (current->vertex == vertex) {
                p = current;
                current = current->childs[i];
                found = true;
                break;
            }
        }

        // 如果不存在则创建新节点
        if (!found) {
            Node* newNode = create_node(vertex, false);
            add_child(p, newNode);
            p = newNode;
        }
    }

    // 标记最后一个节点为叶子
    p->is_leaf = true;

}*/

// 插入排序后的极大团到前缀树（假设 sortedClique[0] == root->vertex）
void insertIntoPrefixTree(Node* root, const std::vector<int>& sortedClique) {
    if (sortedClique.empty() || root->vertex != sortedClique[0]) {
        return;  // 要么空，要么根不匹配，直接返回
    }

    Node* current = root;
    // 从 1 开始，因为 sortedClique[0] 对应 root
    for (size_t i = 1; i < sortedClique.size(); ++i) {
        int vertex = sortedClique[i];
        // 在 current 的子节点中查找 vertex
        Node* child = nullptr;
        for (int j = 0; j < current->child_count; ++j) {
            if (current->childs[j]->vertex == vertex) {
                child = current->childs[j];
                break;
            }
        }

        if (child) {
            // 公共前缀存在，沿子节点往下
            current = child;
        } else {
            // 不存在则新建并加入
            Node* newNode = create_node(vertex, false);
            add_child(current, newNode);
            current = newNode;
        }
    }

    // 最后标记为叶子
    current->is_leaf = true;
}

// 递归计算单个前缀树的内存（保持原核心逻辑）
size_t calculatePrefixTreeMemory(Node* currentNode, size_t* totalNodes) {
    if (!currentNode) return 0;

    size_t nodeMemory = sizeof(Node) + currentNode->child_count * sizeof(Node*);
    (*totalNodes)++;

    for (int i = 0; i < currentNode->child_count; ++i) {
        nodeMemory += calculatePrefixTreeMemory(currentNode->childs[i], totalNodes);
    }

    return nodeMemory;
}

// 分析前缀树的内存占用
void analyzePrefixTreeMemory(Node* root) {
    size_t totalMem = 0;
    size_t totalNodes = 0;

    totalMem = calculatePrefixTreeMemory(root, &totalNodes);

    printf("prefixTree biclique index memory: %.4f MB\n nodes count: %zu\n",
           totalMem/(1024.0*1024.0), totalNodes);
}

// 新版：释放前缀树
void freePrefixTree(Node* root) {
    if (!root) return;

    // 后序遍历释放子节点
    for (int i = 0; i < root->child_count; ++i) {
        freePrefixTree(root->childs[i]);
    }

    // 释放子节点指针数组
    if (root->child_count > 0) {
        free(root->childs);
    }

    // 释放当前节点（包括虚拟根节点）
    free(root);
}

// 辅助函数：合并两个子树
void merge_subtrees(Node* parent, Node* existing_child, Node* new_child) {
    // 遍历new_child的所有子节点
    for (int i = 0; i < new_child->child_count; i++) {
        Node* new_grandchild = new_child->childs[i];
        bool found = false;

        // 检查existing_child是否已有相同顶点子节点
        for (int j = 0; j < existing_child->child_count; j++) {
            if (existing_child->childs[j]->vertex == new_grandchild->vertex) {
                // 递归合并子树
                merge_subtrees(existing_child, existing_child->childs[j], new_grandchild);
                found = true;
                break;
            }
        }

        // 如果没有相同顶点子节点，直接添加
        if (!found) {
            add_child(existing_child, new_grandchild);
        }
    }

    // 处理叶子节点情况
    if (new_child->is_leaf) {
        existing_child->is_leaf = true;
    }

    // 释放已合并的节点（但不释放其子节点）
    free(new_child->childs);
    free(new_child);
}

// 主转换函数：将普通树转换为前缀树
void convert_to_prefix_tree(Node* root) {
    if (!root || root->child_count <= 1) return;

    // 创建一个临时数组存储子节点
    Node** children = (Node**)malloc(root->child_count * sizeof(Node*));
    int child_count = root->child_count;
    memcpy(children, root->childs, child_count * sizeof(Node*));

    // 清空root的子节点列表
    root->child_count = 0;
    free(root->childs);
    root->childs = NULL;

    // 遍历所有子节点进行合并
    for (int i = 0; i < child_count; i++) {
        Node* current_child = children[i];
        bool merged = false;

        // 检查是否已有相同顶点的子节点
        for (int j = 0; j < root->child_count; j++) {
            if (root->childs[j]->vertex == current_child->vertex) {
                // 合并子树
                merge_subtrees(root, root->childs[j], current_child);
                merged = true;
                break;
            }
        }

        // 如果没有相同顶点子节点，直接添加
        if (!merged) {
            add_child(root, current_child);
        }
    }

    free(children);

    // 递归处理所有子节点
    for (int i = 0; i < root->child_count; i++) {
        convert_to_prefix_tree(root->childs[i]);
    }
}

////加边时维护用函数
// 计算两个升序数组的交集
std::vector<int> intersect(const std::vector<Node*>& a, const std::vector<Node*>& b) {
    std::vector<int> res;
    int i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i]->vertex == b[j]->vertex) {
            res.push_back(a[i]->vertex);
            i++;
            j++;
        } else if (a[i]->vertex < b[j]->vertex) {
            i++;
        } else {
            j++;
        }
    }
    return res;
}

// 生成所有唯一的交集序列
std::vector<std::vector<int>> computeAllIntersections(const std::vector<SubtreeResult>& data1, const std::vector<SubtreeResult>& data2) {
    std::set<std::vector<int>> unique_intersections;
    for (const auto& group1 : data1) {
        for (const auto& group2 : data2) {
            auto inter = intersect(group1.path, group2.path);
            if (!inter.empty()) {
                unique_intersections.insert(inter);
            }
        }
    }
    return std::vector<std::vector<int>>(unique_intersections.begin(), unique_intersections.end());
}

// 判断 s 是否是 t 的子序列
bool isSubsequence(const std::vector<int>& s, const std::vector<int>& t) {
    int i = 0;
    for (int x : t) {
        if (i < s.size() && x == s[i]) {
            i++;
        }
    }
    return i == s.size();
}

// 过滤被包含的序列
std::vector<std::vector<int>> filterNonContained(const std::vector<std::vector<int>>& all_intersections) {
    std::vector<std::vector<int>> sorted = all_intersections;
    std::sort(sorted.begin(), sorted.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
        if (a.size() != b.size()) {
            return a.size() > b.size();  // 长度降序
        } else {
            return a < b;  // 字典序升序
        }
    });

    std::vector<std::vector<int>> result;
    for (const auto& s : sorted) {
        bool is_contained = false;
        for (const auto& t : result) {
            if (isSubsequence(s, t)) {
                is_contained = true;
                break;
            }
        }
        if (!is_contained) {
            result.push_back(s);
        }
    }
    return result;
}

// 辅助函数：深度优先遍历并收集完整子树路径
void collectSubtreePaths(Node* node, int target, Node *subtreeRoot, std::vector<Node*>& currentPath,
                         std::vector<SubtreeResult>& result, bool& targetFound) {
    if (!node) return;

    // 当前路径添加节点
    currentPath.push_back(node);
    //currentPath.push_back(node->vertex);

    // 检查是否找到目标顶点
    if (node->vertex == target) {
        targetFound = true;
    }

    // 如果是叶子节点或已经找到目标，收集完整路径
    if (node->is_leaf) {
        if (targetFound) {
            result.push_back({subtreeRoot, currentPath});
        }
    }
    else {
        // 递归处理所有子节点
        for (int i = 0; i < node->child_count; ++i) {
            bool childFound = targetFound;  // 保持当前找到状态
            collectSubtreePaths(node->childs[i], target, subtreeRoot, currentPath, result, childFound);
            targetFound = targetFound || childFound;  // 合并子节点找到状态
        }
    }

    // 回溯前处理找到状态
    if (node->vertex == target) {
        targetFound = false;
    }
    currentPath.pop_back();
}

// 找到插入边2个顶点的所有树分支序列
std::pair<std::vector<SubtreeResult>, std::vector<SubtreeResult>>
findVertexSubtreePaths(Node* root, int v1, int v2) {
    std::vector<SubtreeResult> paths1, paths2;
    std::vector<Node*> currentPath;
    bool found = false;

    // 处理第一个顶点
    for (int i = 0; i < root->child_count; ++i) {
        found = false;
        collectSubtreePaths(root->childs[i], v1, root->childs[i], currentPath, paths1, found);
        currentPath.clear();
    }

    // 处理第二个顶点
    for (int i = 0; i < root->child_count; ++i) {
        found = false;
        collectSubtreePaths(root->childs[i], v2, root->childs[i], currentPath, paths2, found);
        currentPath.clear();
    }

    return {paths1, paths2};
}

// ------------------- 冗余删除与新 clique 插入函数 ------------------- 针对添加边的情况
// 判断给定路径（vector<Node*>）中的所有 vertex 是否都出现在 newClique 中
bool pathContainedInNewClique(const std::vector<Node*>& path, const std::vector<int>& newClique) {
    if (path.empty()){
        return false;
    }
    for (Node* nd : path) {
        // 如果该节点的 vertex 不在 newClique 中，则返回 false
        if (std::find(newClique.begin(), newClique.end(), nd->vertex) == newClique.end())
            return false;
    }
    return true;
}

// 删除冗余：从路径末尾开始，如果当前节点的 vertex 属于 newClique 且该节点没有子节点，则删除；否则停止删除
/*void removeRedundantSuffix(SubtreeResult& subtreeRes, const std::vector<int>& newClique) {
    while (!subtreeRes.path.empty()) {
        Node* lastNode = subtreeRes.path.back();
        // 如果 lastNode->vertex 属于 newClique 且该节点没有子节点则可删除
        if (std::find(newClique.begin(), newClique.end(), lastNode->vertex) != newClique.end() &&
            lastNode->child_count == 0)
        {
            printf("remove node: %d ", lastNode->vertex);
            printf("remove node childcount: %d ", lastNode->child_count);
            //free(lastNode);
            printf("old back: %d ", subtreeRes.path.back()->vertex);
            printf("old back childcount: %d ", subtreeRes.path.back()->child_count);
            subtreeRes.path.pop_back();
            subtreeRes.path.back()->child_count--;
            printf("new back: %d ", subtreeRes.path.back()->vertex);
            printf("new back childcount: %d ", subtreeRes.path.back()->child_count);
        } else {
            break;
        }
    }
}*/

void removeRedundantSuffix(SubtreeResult& subtreeRes, const std::vector<int>& newClique) {
    auto& path = subtreeRes.path;
    // 只要 path 不空，就检查最后一个节点
    while (!path.empty()) {
        Node* lastNode = path.back();
        // 如果它在 newClique 中，且没有子节点，则可以删除
        if (std::find(newClique.begin(), newClique.end(), lastNode->vertex) != newClique.end()
            && lastNode->child_count == 0)
        {
            // 找到它的父节点（path 中倒数第二个元素，或者 root）
            Node* parent = (path.size() >= 2 ? path[path.size()-2] : subtreeRes.root);

            // 在 parent->childs[] 中找到 lastNode 的下标 idx
            int idx = -1;
            for (int i = 0; i < parent->child_count; i++) {
                if (parent->childs[i] == lastNode) {
                    idx = i;
                    break;
                }
            }
            assert(idx != -1);

            // 释放 lastNode 本身（它的 childs 一定是空的）
            free(lastNode);

            // 从 parent->childs 数组中删掉这个指针
            for (int j = idx; j < parent->child_count - 1; j++) {
                parent->childs[j] = parent->childs[j+1];
            }
            parent->child_count--;
            if (parent->child_count == 0) {
                free(parent->childs);
                parent->childs = nullptr;
            } else {
                parent->childs = (Node**)realloc(parent->childs, sizeof(Node*) * parent->child_count);
            }

            // 最后，从 path 向量里 pop 掉这个节点
            path.pop_back();
        }
        else {
            // 要么不在 newClique 中，要么还有子节点，停止删除
            break;
        }
    }
}

// 对于 result 中的每个新 clique，先在所有待处理的前缀树（paths1 和 paths2）中进行冗余删除，
// 然后如果新 clique 的首元素与某个前缀树的 root->vertex 相同，则以该前缀树为目标插入新 clique（且只插入一次）。
void updatePrefixTrees_forAdd(std::vector<SubtreeResult>& paths1,
                       std::vector<SubtreeResult>& paths2,
                       const std::vector<std::vector<int>>& result, Node *prefixTreeRoot) {
    // 对于每一个新 clique
    for (const auto& newClique : result) {
//        for (int v : newClique) { printf("%d ", v);}
//        printf("\n");
        //// 删除存在问题
        // 首先：对所有前缀树中的路径进行冗余删除
        for (auto& subtreeRes : paths1) {
//            for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//            printf("\n");
            if (pathContainedInNewClique(subtreeRes.path, newClique)) {
//                printf("1.remove\n");
//                for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//                printf("\n");
                removeRedundantSuffix(subtreeRes, newClique);
                subtreeRes.path.clear();//删除冗余路径的记录
            }
        }
        for (auto& subtreeRes : paths2) {
//            for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//            printf("\n");
            if (pathContainedInNewClique(subtreeRes.path, newClique)) {
//                printf("2.remove\n");
//                for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//                printf("\n");
                removeRedundantSuffix(subtreeRes, newClique);
                subtreeRes.path.clear();//删除冗余路径的记录
            }
        }
        //printf("after remove:\n");
        //print_tree(prefixTreeRoot, 0);
        // 然后：在符合插入条件的前缀树中插入新 clique
        // 条件：newClique 的首元素必须与前缀树的 root->vertex 相同
        bool inserted = false;
        // 处理 paths1
        for (auto& subtreeRes : paths1) {
            if (!newClique.empty() && subtreeRes.root->vertex == newClique[0]) {
                //insertNewClique(subtreeRes, newClique);
                insertIntoPrefixTree(subtreeRes.root, newClique);////
                inserted = true;
                break;
            }
        }
        // 如果 paths1 中未找到，则在 paths2 中查找并插入
        if (!inserted) {
            for (auto& subtreeRes : paths2) {
                if (!newClique.empty() && subtreeRes.root->vertex == newClique[0]) {
                    //insertNewClique(subtreeRes, newClique);
                    insertIntoPrefixTree(subtreeRes.root, newClique);
                    inserted = true;
                    break;
                }
            }
        }
        //printf("after insert:\n");
        //print_tree(prefixTreeRoot, 0);
    }
}

//添加边维护
void addEdgeMaintain(Node *prefixTreeRoot) {
    std::string filePath = "D:\\school\\second year\\First Semester\\biclique index\\datasets\\sc-pkustk11_add.txt";
    std::vector<std::pair<int, int>> addEdge = readPairsFromFile(filePath);
    for (auto e : addEdge) {
        int v1 = e.first;
        int v2 = e.second;
        auto [paths1, paths2] = findVertexSubtreePaths(prefixTreeRoot, v1, v2);
        // 计算所有交集
        auto all_intersections = computeAllIntersections(paths1, paths2);
        // 找到新增的极大团
        auto result = filterNonContained(all_intersections);
        for (int i = 0; i < result.size(); ++i) {
            result[i].push_back(v1);
            result[i].push_back(v2);
            std::sort(result[i].begin(), result[i].end());
        }
        // 先删除冗余树序列，再将新极大团插入，维持前缀树结构
        updatePrefixTrees_forAdd(paths1, paths2, result, prefixTreeRoot);
    }
}

/**
 * node          当前遍历到的节点
 * subtreeRoot   本次 DFS 所在子树的根（虚拟根的直接子节点）
 * v0, v1        目标顶点
 * has0,has1     到目前为止路径中是否已见到 v0/v1
 * currentPath   从 subtreeRoot 开始到 node 的节点序列
 * bothOut       同时包含 v0 和 v1 的分支结果
 * eitherOut     包含 v0 或 v1 任意一个的分支结果
 */
void deleteTree_collect(
        Node* node,
        Node* subtreeRoot,
        int v0,
        int v1,
        bool has0,
        bool has1,
        std::vector<Node*>& currentPath,
        std::vector<SubtreeResult>& bothOut,
        std::vector<SubtreeResult>& eitherOut)
{
    // 如果是叶节点，做记录
    if (node->is_leaf) {
        // 同时包含 v0 和 v1：record to bothOut, path exclude v0/v1
        if (has0 && has1) {
            SubtreeResult sr;
            sr.root = subtreeRoot;
            sr.path = currentPath;  // copy entire path
//            sr.path.reserve(currentPath.size());
//            for (Node* nd : currentPath) {
//                int w = nd->vertex;
//                if (w != v0 && w != v1) {
//                    sr.path.push_back(nd);
//                }
//            }
            bothOut.push_back(std::move(sr));
        }
        // 包含 v0 或 v1：record to eitherOut, path is full currentPath
        else if (has0 || has1) {
            SubtreeResult sr;
            sr.root = subtreeRoot;
            sr.path = currentPath;  // copy entire path
            eitherOut.push_back(std::move(sr));
        }
        return;
    }

    // 继续遍历子节点
    for (int i = 0; i < node->child_count; ++i) {
        Node* ch = node->childs[i];
        bool saw0 = has0 || (ch->vertex == v0);
        bool saw1 = has1 || (ch->vertex == v1);

        currentPath.push_back(ch);
        deleteTree_collect(ch, subtreeRoot, v0, v1, saw0, saw1,
                   currentPath, bothOut, eitherOut);
        currentPath.pop_back();
    }
}

/**
 * 遍历前缀树（虚拟根下的每棵子树），收集：
 *  1) 同时包含 v0 和 v1 的分支
 *  2) 包含 v0 或 v1 的分支
 *
 * @param prefixTreeRoot 虚拟根（vertex == -1）
 * @param v0, v1         目标顶点
 * @param bothOut        输出：同时包含 v0&v1 的 SubtreeResult 列表
 * @param eitherOut      输出：包含 v0 或 v1 的 SubtreeResult 列表
 */
void findBranchesForDelete(Node* prefixTreeRoot, int v0, int v1, std::vector<SubtreeResult>& bothOut, std::vector<SubtreeResult>& eitherOut)
{
    int n = prefixTreeRoot->child_count;
    bothOut.reserve(n);
    eitherOut.reserve(n);
    //eitherOut.reserve(n * 2);
    // 对虚拟根下的每个子树根执行 DFS
    for (int i = 0; i < n; ++i) {
        Node* subtreeRoot = prefixTreeRoot->childs[i];
        bool has0 = (subtreeRoot->vertex == v0);
        bool has1 = (subtreeRoot->vertex == v1);

        std::vector<Node*> path;
        path.reserve(32);
        path.push_back(subtreeRoot);

        deleteTree_collect(subtreeRoot, subtreeRoot, v0, v1, has0, has1,
                   path, bothOut, eitherOut);
    }
}

// ———— 1) 生成新的 clique：在每条 path 中分别插入 v0、v1 ————
std::vector<std::vector<int>> generateAugmentedCliques_delete(const std::vector<SubtreeResult>& bothOut,
                         int v0,
                         int v1)
{
    std::vector<std::vector<int>> result;
    result.reserve(bothOut.size() * 2);
    for (auto const& sr : bothOut) {
        // 抽取 base path
        std::vector<int> base;
        base.reserve(sr.path.size() - 2);////
        for (Node* nd : sr.path) {
            if (nd->vertex != v0 && nd->vertex != v1) {////
                base.push_back(nd->vertex);
            }
        }
        // 插 v0
        {
            std::vector<int> c;
            c.reserve(base.size() + 1);
            bool done = false;
            for (int x : base) {
                if (!done && v0 < x) {
                    c.push_back(v0);
                    done = true;
                }
                c.push_back(x);
            }
            if (!done) c.push_back(v0);
            result.push_back(std::move(c));
        }
        // 插 v1
        {
            std::vector<int> c;
            c.reserve(base.size() + 1);
            bool done = false;
            for (int x : base) {
                if (!done && v1 < x) {
                    c.push_back(v1);
                    done = true;
                }
                c.push_back(x);
            }
            if (!done) c.push_back(v1);
            result.push_back(std::move(c));
        }
    }
    return result;
}

// ———— 辅助：删除 parent 下的 child 节点 ————
void removeChild_delete(Node* parent, Node* child) {
    int idx = -1;
    for (int i = 0; i < parent->child_count; ++i) {
        if (parent->childs[i] == child) {
            idx = i;
            break;
        }
    }
    assert(idx >= 0);
    // 释放 child 本身及其子指针数组（假设 child->child_count == 0）
    free(child);
    // 向前覆盖
    for (int j = idx; j + 1 < parent->child_count; ++j) {
        parent->childs[j] = parent->childs[j+1];
    }
    parent->child_count--;
    if (parent->child_count > 0) {
        parent->childs = (Node**)realloc(parent->childs,
                                         sizeof(Node*) * parent->child_count);
    } else {
        free(parent->childs);
        parent->childs = nullptr;
    }
}

// ———— 2) 从后往前删除 bothOut 中各 path 的冗余节点 ————
void prunePaths_delete(std::vector<SubtreeResult>& bothOut, Node* prefixTreeRoot) {
    for (auto& sr : bothOut) {
        auto& path = sr.path;
        // 从末尾向前，跳过根节点 index 0
        for (int i = (int)path.size() - 1; i >= 0; --i) {
            Node* nd = path[i];
            // 只有当它是叶子（child_count==0）才删除
            if (nd->child_count != 0) {
                break;
            }
            Node* parent;
            if (i == 0) {
                parent =  prefixTreeRoot;
            }
            else {
                parent = path[i-1];
            }
//            printf("parent: %d\n", parent->vertex);
//            printf("child: %d\n", nd->vertex);
            removeChild_delete(parent, nd);
            // 从 path 中弹出
//            printf("the back: %d\n", path.back()->vertex);
            path.pop_back();
        }
    }
}

// 判断 subseq 是否是 seq 的子序列（按顺序，不必连续）
bool isSubsequence_delete(const std::vector<int>& seq, const std::vector<int>& subseq) {
    size_t i = 0, j = 0;
    while (i < seq.size() && j < subseq.size()) {
        if (seq[i] == subseq[j]) ++j;
        ++i;
    }
    return j == subseq.size();
}

/**
 * 将所有新的 clique 序列插入到前缀树（挂到 eitherOut 指定的子树或虚拟根）中，
 * 但跳过那些已被 eitherOut 中某条 path 包含的 clique。
 *
 * @param result            新的 clique 列表，每条已按升序排列
 * @param eitherOut         中间存储：保存了若干 SubtreeResult，其中 .path 用于检测包含关系，
 *                          但本函数不修改 eitherOut 本身
 * @param prefixTreeRoot    前缀树的虚拟根（vertex == -1）
 */
// ———— 3) 将删除边后产生的新极大团插入到索引里 ————
void insertResultIntoEither_delete(
        const std::vector<std::vector<int>>& result,
        const std::vector<SubtreeResult>& eitherOut,
        Node* prefixTreeRoot)
{
    for (auto const& clique : result) {
        if (clique.empty()) continue;

        // 1) 如果已被某条 existing path 包含（是子序列），则跳过
        bool skip = false;
        for (auto const& sr : eitherOut) {
            // 从 sr.path 构造它的顶点序列
            std::vector<int> existing;
            existing.reserve(sr.path.size());
            for (auto nd : sr.path) {
                existing.push_back(nd->vertex);
            }
            if (isSubsequence_delete(existing, clique)) {
                skip = true;
                break;
            }
        }
        if (skip) continue;

        // 2) 查找 eitherOut 中是否有匹配 clique[0] 的子树根
        int rootVal = clique[0];
        bool inserted = false;
        for (auto const& sr : eitherOut) {
            if (sr.root->vertex == rootVal) {
                // 挂到 sr.root 下
                insertIntoPrefixTree(sr.root, clique);
                inserted = true;
                break;
            }
        }

        // 3) 如果没有匹配的子树根，就挂到虚拟根 prefixTreeRoot 下
        if (!inserted) {
            Node* newRoot = create_node(rootVal, false);
            add_child(prefixTreeRoot, newRoot);
            insertIntoPrefixTree(newRoot, clique);
        }
    }
}

std::vector<std::pair<int, int>> readPairsFromFile(const std::string& filePath) {
    std::vector<std::pair<int, int>> result;
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return result; // 返回空vector
    }
    int first, second, weight; // 第三列权重虽然读取但不使用
    while (inputFile >> first >> second >> weight) {
        result.emplace_back(first, second);
    }
    inputFile.close();
    return result;
}

void deleteEdgeMaintain(Node *prefixTreeRoot) {
    std::string filePath = "D:\\school\\second year\\First Semester\\biclique index\\datasets\\p_hat1500-1_delete.txt";
    std::vector<std::pair<int, int>> deleteEdges = readPairsFromFile(filePath);
    for (auto e : deleteEdges) {
        int v1 = e.first;
        int v2 = e.second;
        std::vector<SubtreeResult> bothOut;
        std::vector<SubtreeResult> eitherOut;
        findBranchesForDelete(prefixTreeRoot, v1, v2, bothOut, eitherOut);
        // 1) 生成新 cliques
        auto result = generateAugmentedCliques_delete(bothOut, v1, v2);
        // 2) 删除原 bothOut 路径上的冗余节点
        prunePaths_delete(bothOut, prefixTreeRoot);
        // 3) 将新 cliques 插入到 eitherOut 中
        insertResultIntoEither_delete(result, eitherOut, prefixTreeRoot);
    }
}

void removeSubtree(Node* virtualRoot, Node* subtreeRoot) {
    int n = virtualRoot->child_count;
    for (int i = 0; i < n; ++i) {
        if (virtualRoot->childs[i] == subtreeRoot) {
            // 先释放这一整条链
            free_tree(subtreeRoot);
            // 用最后一个子树指针覆盖
            virtualRoot->childs[i] = virtualRoot->childs[n - 1];
            // 缩减计数
            virtualRoot->child_count--;
            return;
        }
    }
}

//普通树加边
void updateTrees_forAdd(std::vector<SubtreeResult>& paths1,
                        std::vector<SubtreeResult>& paths2,
                        const std::vector<std::vector<int>>& result, Node* prefixTreeRoot) {
    // 对于每一个新 clique
    for (const auto& newClique : result) {
//        for (int v : newClique) { printf("%d ", v);}
//        printf("\n");
        // 首先：对所有前缀树中的路径进行冗余删除
        for (auto& subtreeRes : paths1) {
//            for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//            printf("\n");
            if (subtreeRes.root && pathContainedInNewClique(subtreeRes.path, newClique)) {
//                printf("1.remove\n");
//                for (Node *v : subtreeRes.path) { printf("%d %d ", v->vertex, v->child_count);}
//                printf("\n");
                //removeRedundantSuffix(subtreeRes, newClique);
                //直接删除冗余的树分支
                removeSubtree(prefixTreeRoot, subtreeRes.root);
                subtreeRes.root = nullptr;
                subtreeRes.path.clear();//删除冗余路径的记录
            }
        }
        for (auto& subtreeRes : paths2) {
//            for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//            printf("\n");
            if (subtreeRes.root && pathContainedInNewClique(subtreeRes.path, newClique)) {
//                printf("2.remove\n");
//                for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//                printf("\n");
                //removeRedundantSuffix(subtreeRes, newClique);
                //直接删除冗余的树分支
                removeSubtree(prefixTreeRoot, subtreeRes.root);
                subtreeRes.root = nullptr;
                subtreeRes.path.clear();//删除冗余路径的记录
            }
        }
//        printf("after remove:\n");
//        print_tree(prefixTreeRoot, 0);
        // 然后：在虚拟根节点下插入新 clique
        // 处理 paths1
        Node *current = prefixTreeRoot;
        for (int v : newClique) {
            Node *newNode = create_node(v, false);
            add_child(current, newNode);
            current = newNode;
        }
        current->is_leaf = true;
        //printf("after insert:\n");
        //print_tree(prefixTreeRoot, 0);
    }
}

//添加边维护,普通树
void addEdgeMaintain_Tree(Node *prefixTreeRoot, const char* edgeFilePath) {
    std::string filePath(edgeFilePath);
    std::vector<std::pair<int, int>> addEdge = readPairsFromFile(filePath);
    for (auto e : addEdge) {
        int v1 = e.first;
        int v2 = e.second;
       //定位函数
        //auto start1 = std::chrono::high_resolution_clock::now();
        auto [paths1, paths2] = findVertexSubtreePaths(prefixTreeRoot, v1, v2);
        //auto end1 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> duration1 = end1 - start1;
        //std::cout << "findVertexSubtreePaths execution time: " << duration1.count()  << " seconds" << std::endl;

        //统计操作的树节点数量
//        int treeNode = 0;
//        for (const auto &item: paths1) {
//            treeNode += item.path.size();
//        }
//        for (const auto &item: paths2) {
//            treeNode += item.path.size();
//        }

        //具体维护过程
        // 计算所有交集
        //auto start2 = std::chrono::high_resolution_clock::now();
        auto all_intersections = computeAllIntersections(paths1, paths2);
        // 找到新增的极大团
        auto result = filterNonContained(all_intersections);
        for (int i = 0; i < result.size(); ++i) {
            result[i].push_back(v1);
            result[i].push_back(v2);
            //treeNode += result[i].size();
            std::sort(result[i].begin(), result[i].end());
        }
        //std::cout << "the all used tree node number : " << treeNode << std::endl;
        // 先删除冗余树序列，再将新极大团插入，维持前缀树结构
        updateTrees_forAdd(paths1, paths2, result, prefixTreeRoot);
        //auto end2 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> duration2 = end2 - start2;
        //std::cout << "Maintenance process execution time: " << duration2.count() / 1000000 << " seconds" << std::endl;
    }
}

//普通树,删除边下,将不冗余的结果插入
void insertResultIntoEither_TreeDelete(const std::vector<std::vector<int>>& result, const std::vector<SubtreeResult>& eitherOut, Node* prefixTreeRoot) {
    for (auto const& clique : result) {
        if (clique.empty()) continue;

        // 1) 如果已被某条 existing path 包含（是子序列），则跳过
        bool skip = false;
        for (auto const& sr : eitherOut) {
            // 从 sr.path 构造它的顶点序列
            std::vector<int> existing;
            existing.reserve(sr.path.size());
            for (auto nd : sr.path) {
                existing.push_back(nd->vertex);
            }
            if (isSubsequence_delete(existing, clique)) {
                skip = true;
                break;
            }
        }
        if (skip) continue;

        // 2) 直接挂到虚拟根 prefixTreeRoot 下
        Node *current = prefixTreeRoot;
        for (int v : clique) {
            Node *newNode = create_node(v, false);
            add_child(current, newNode);
            current = newNode;
        }
        current->is_leaf = true;

    }
}

//删除边维护,普通树
void deleteEdgeMaintain_Tree(Node *prefixTreeRoot, const char* edgeFilePath) {
    std::string filePath(edgeFilePath);
    std::vector<std::pair<int, int>> deleteEdges = readPairsFromFile(filePath);
    for (auto e : deleteEdges) {
        int v1 = e.first;
        int v2 = e.second;
        std::vector<SubtreeResult> bothOut;
        std::vector<SubtreeResult> eitherOut;

	//统计操作的树节点数量
//        int treeNode = 0;
//        for (const auto &item: bothOut) {
//            treeNode += item.path.size();
//        }
//        for (const auto &item: eitherOut) {
//            treeNode += item.path.size();
//        }

        //auto start1 = std::chrono::high_resolution_clock::now();
        //定位函数
        findBranchesForDelete(prefixTreeRoot, v1, v2, bothOut, eitherOut);
        //auto end1 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> duration1 = end1 - start1;
        //std::cout << "findBranchesForDelete execution time: " << duration1.count()  << " seconds" << std::endl;

        //auto start2 = std::chrono::high_resolution_clock::now();
        //具体维护过程
        // 1) 生成新 cliques
        auto result = generateAugmentedCliques_delete(bothOut, v1, v2);

        for (int i = 0; i < result.size(); ++i) {
            //treeNode += result[i].size();
        }
        //std::cout << "the all used tree node number : " << treeNode << std::endl;

        // 2) 删除原 bothOut 路径上的冗余节点
        for (auto sr : bothOut) {
            removeSubtree(prefixTreeRoot, sr.root);
        }
        // 3) 将新 cliques 插入到 eitherOut 中
        insertResultIntoEither_TreeDelete(result, eitherOut, prefixTreeRoot);

        //auto end2 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> duration2 = end2 - start2;
        //std::cout << "Maintenance process execution time: " << duration2.count()  << " seconds" << std::endl;
    }
}