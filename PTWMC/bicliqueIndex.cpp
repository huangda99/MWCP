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

Node* create_node(int vertex, bool is_leaf) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->is_leaf = is_leaf;
    newNode->vertex = vertex;
    newNode->childs = NULL;
    newNode->child_count = 0;
    return newNode;
}


void add_child(Node* parent, Node* child) {
    parent->child_count++;
    parent->childs = (Node**)realloc(parent->childs, sizeof(Node*) * parent->child_count);
    parent->childs[parent->child_count - 1] = child;
}

void print_tree(Node* node, int depth) {
    if(!node) return;

    for(int i=0; i<depth; i++) printf("  ");
    printf("%d%s\n", node->vertex, node->is_leaf ? " (leaf)" : "");

    for(int i=0; i<node->child_count; i++) {
        print_tree(node->childs[i], depth+1);
    }
}


int addWeightForBiclique(Node* node, std::vector<float> &vertexWeight) {
    if (!node) return 0;
    float currentWeight = vertexWeight[node->vertex];
    for(int i = 0; i < node->child_count; i++) {
        currentWeight += addWeightForBiclique(node->childs[i], vertexWeight);
    }
    return currentWeight;
}


size_t calculateTreeMemory(Node* node, size_t* total_nodes) {
    if (!node) return 0;


    size_t node_memory = sizeof(Node) + node->child_count * sizeof(Node*);


    (*total_nodes)++;


    for (int i=0; i<node->child_count; ++i) {
        node_memory += calculateTreeMemory(node->childs[i], total_nodes);
    }

    return node_memory;
}


void analyzeMemory(Node** roots, int rootCount) {
    size_t total_mem = 0;
    size_t total_nodes = 0;

    for (int i=0; i<rootCount; ++i) {
        total_mem += calculateTreeMemory(roots[i], &total_nodes);
    }

    printf("biclique index total memory: %.4f MB\n number of nodes: %zu\n",
           total_mem/(1024.0*1024.0), total_nodes);
}


void free_tree(Node* root) {
    if(!root) return;
    for(int i=0; i<root->child_count; i++) {
        free_tree(root->childs[i]);
    }
    free(root->childs);
    free(root);
}


/*void insertIntoPrefixTree(Node* root, const std::vector<int>& sortedClique) {
    Node* current = root;
    Node *p = root;
    for (int vertex : sortedClique) {
        bool found = false;

        for (int i = 0; i < current->child_count; ++i) {
            //if (current->childs[i]->vertex == vertex) {
            if (current->vertex == vertex) {
                p = current;
                current = current->childs[i];
                found = true;
                break;
            }
        }


        if (!found) {
            Node* newNode = create_node(vertex, false);
            add_child(p, newNode);
            p = newNode;
        }
    }


    p->is_leaf = true;

}*/


void insertIntoPrefixTree(Node* root, const std::vector<int>& sortedClique) {
    if (sortedClique.empty() || root->vertex != sortedClique[0]) {
        return;  
    }

    Node* current = root;

    for (size_t i = 1; i < sortedClique.size(); ++i) {
        int vertex = sortedClique[i];

        Node* child = nullptr;
        for (int j = 0; j < current->child_count; ++j) {
            if (current->childs[j]->vertex == vertex) {
                child = current->childs[j];
                break;
            }
        }

        if (child) {

            current = child;
        } else {

            Node* newNode = create_node(vertex, false);
            add_child(current, newNode);
            current = newNode;
        }
    }

    current->is_leaf = true;
}


size_t calculatePrefixTreeMemory(Node* currentNode, size_t* totalNodes) {
    if (!currentNode) return 0;

    size_t nodeMemory = sizeof(Node) + currentNode->child_count * sizeof(Node*);
    (*totalNodes)++;

    for (int i = 0; i < currentNode->child_count; ++i) {
        nodeMemory += calculatePrefixTreeMemory(currentNode->childs[i], totalNodes);
    }

    return nodeMemory;
}


void analyzePrefixTreeMemory(Node* root) {
    size_t totalMem = 0;
    size_t totalNodes = 0;

    totalMem = calculatePrefixTreeMemory(root, &totalNodes);

    printf("prefixTree biclique index memory: %.4f MB\n nodes count: %zu\n",
           totalMem/(1024.0*1024.0), totalNodes);
}


void freePrefixTree(Node* root) {
    if (!root) return;


    for (int i = 0; i < root->child_count; ++i) {
        freePrefixTree(root->childs[i]);
    }


    if (root->child_count > 0) {
        free(root->childs);
    }

    free(root);
}


void merge_subtrees(Node* parent, Node* existing_child, Node* new_child) {

    for (int i = 0; i < new_child->child_count; i++) {
        Node* new_grandchild = new_child->childs[i];
        bool found = false;

        for (int j = 0; j < existing_child->child_count; j++) {
            if (existing_child->childs[j]->vertex == new_grandchild->vertex) {

                merge_subtrees(existing_child, existing_child->childs[j], new_grandchild);
                found = true;
                break;
            }
        }


        if (!found) {
            add_child(existing_child, new_grandchild);
        }
    }


    if (new_child->is_leaf) {
        existing_child->is_leaf = true;
    }


    free(new_child->childs);
    free(new_child);
}


void convert_to_prefix_tree(Node* root) {
    if (!root || root->child_count <= 1) return;


    Node** children = (Node**)malloc(root->child_count * sizeof(Node*));
    int child_count = root->child_count;
    memcpy(children, root->childs, child_count * sizeof(Node*));


    root->child_count = 0;
    free(root->childs);
    root->childs = NULL;


    for (int i = 0; i < child_count; i++) {
        Node* current_child = children[i];
        bool merged = false;


        for (int j = 0; j < root->child_count; j++) {
            if (root->childs[j]->vertex == current_child->vertex) {

                merge_subtrees(root, root->childs[j], current_child);
                merged = true;
                break;
            }
        }


        if (!merged) {
            add_child(root, current_child);
        }
    }

    free(children);


    for (int i = 0; i < root->child_count; i++) {
        convert_to_prefix_tree(root->childs[i]);
    }
}

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


bool isSubsequence(const std::vector<int>& s, const std::vector<int>& t) {
    int i = 0;
    for (int x : t) {
        if (i < s.size() && x == s[i]) {
            i++;
        }
    }
    return i == s.size();
}


std::vector<std::vector<int>> filterNonContained(const std::vector<std::vector<int>>& all_intersections) {
    std::vector<std::vector<int>> sorted = all_intersections;
    std::sort(sorted.begin(), sorted.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
        if (a.size() != b.size()) {
            return a.size() > b.size();  
        } else {
            return a < b;  
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

void collectSubtreePaths(Node* node, int target, Node *subtreeRoot, std::vector<Node*>& currentPath,
                         std::vector<SubtreeResult>& result, bool& targetFound) {
    if (!node) return;


    currentPath.push_back(node);
    //currentPath.push_back(node->vertex);


    if (node->vertex == target) {
        targetFound = true;
    }


    if (node->is_leaf) {
        if (targetFound) {
            result.push_back({subtreeRoot, currentPath});
        }
    }
    else {

        for (int i = 0; i < node->child_count; ++i) {
            bool childFound = targetFound; 
            collectSubtreePaths(node->childs[i], target, subtreeRoot, currentPath, result, childFound);
            targetFound = targetFound || childFound;  
        }
    }


    if (node->vertex == target) {
        targetFound = false;
    }
    currentPath.pop_back();
}


std::pair<std::vector<SubtreeResult>, std::vector<SubtreeResult>>
findVertexSubtreePaths(Node* root, int v1, int v2) {
    std::vector<SubtreeResult> paths1, paths2;
    std::vector<Node*> currentPath;
    bool found = false;


    for (int i = 0; i < root->child_count; ++i) {
        found = false;
        collectSubtreePaths(root->childs[i], v1, root->childs[i], currentPath, paths1, found);
        currentPath.clear();
    }

    for (int i = 0; i < root->child_count; ++i) {
        found = false;
        collectSubtreePaths(root->childs[i], v2, root->childs[i], currentPath, paths2, found);
        currentPath.clear();
    }

    return {paths1, paths2};
}



bool pathContainedInNewClique(const std::vector<Node*>& path, const std::vector<int>& newClique) {
    if (path.empty()){
        return false;
    }
    for (Node* nd : path) {

        if (std::find(newClique.begin(), newClique.end(), nd->vertex) == newClique.end())
            return false;
    }
    return true;
}


/*void removeRedundantSuffix(SubtreeResult& subtreeRes, const std::vector<int>& newClique) {
    while (!subtreeRes.path.empty()) {
        Node* lastNode = subtreeRes.path.back();

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

    while (!path.empty()) {
        Node* lastNode = path.back();

        if (std::find(newClique.begin(), newClique.end(), lastNode->vertex) != newClique.end()
            && lastNode->child_count == 0)
        {

            Node* parent = (path.size() >= 2 ? path[path.size()-2] : subtreeRes.root);


            int idx = -1;
            for (int i = 0; i < parent->child_count; i++) {
                if (parent->childs[i] == lastNode) {
                    idx = i;
                    break;
                }
            }
            assert(idx != -1);


            free(lastNode);


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


            path.pop_back();
        }
        else {

            break;
        }
    }
}

void updatePrefixTrees_forAdd(std::vector<SubtreeResult>& paths1,
                       std::vector<SubtreeResult>& paths2,
                       const std::vector<std::vector<int>>& result, Node *prefixTreeRoot) {

    for (const auto& newClique : result) {
//        for (int v : newClique) { printf("%d ", v);}
//        printf("\n");

        for (auto& subtreeRes : paths1) {
//            for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//            printf("\n");
            if (pathContainedInNewClique(subtreeRes.path, newClique)) {
//                printf("1.remove\n");
//                for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//                printf("\n");
                removeRedundantSuffix(subtreeRes, newClique);
                subtreeRes.path.clear();
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
                subtreeRes.path.clear();
            }
        }
        //printf("after remove:\n");
        //print_tree(prefixTreeRoot, 0);

        bool inserted = false;

        for (auto& subtreeRes : paths1) {
            if (!newClique.empty() && subtreeRes.root->vertex == newClique[0]) {
                //insertNewClique(subtreeRes, newClique);
                insertIntoPrefixTree(subtreeRes.root, newClique);////
                inserted = true;
                break;
            }
        }

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


void addEdgeMaintain(Node *prefixTreeRoot) {
    std::string filePath = "D:\\school\\second year\\First Semester\\biclique index\\datasets\\sc-pkustk11_add.txt";
    std::vector<std::pair<int, int>> addEdge = readPairsFromFile(filePath);
    for (auto e : addEdge) {
        int v1 = e.first;
        int v2 = e.second;
        auto [paths1, paths2] = findVertexSubtreePaths(prefixTreeRoot, v1, v2);

        auto all_intersections = computeAllIntersections(paths1, paths2);

        auto result = filterNonContained(all_intersections);
        for (int i = 0; i < result.size(); ++i) {
            result[i].push_back(v1);
            result[i].push_back(v2);
            std::sort(result[i].begin(), result[i].end());
        }

        updatePrefixTrees_forAdd(paths1, paths2, result, prefixTreeRoot);
    }
}

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

    if (node->is_leaf) {

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

        else if (has0 || has1) {
            SubtreeResult sr;
            sr.root = subtreeRoot;
            sr.path = currentPath;  // copy entire path
            eitherOut.push_back(std::move(sr));
        }
        return;
    }


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


void findBranchesForDelete(Node* prefixTreeRoot, int v0, int v1, std::vector<SubtreeResult>& bothOut, std::vector<SubtreeResult>& eitherOut)
{
    int n = prefixTreeRoot->child_count;
    bothOut.reserve(n);
    eitherOut.reserve(n);
    //eitherOut.reserve(n * 2);

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


std::vector<std::vector<int>> generateAugmentedCliques_delete(const std::vector<SubtreeResult>& bothOut,
                         int v0,
                         int v1)
{
    std::vector<std::vector<int>> result;
    result.reserve(bothOut.size() * 2);
    for (auto const& sr : bothOut) {

        std::vector<int> base;
        base.reserve(sr.path.size() - 2);////
        for (Node* nd : sr.path) {
            if (nd->vertex != v0 && nd->vertex != v1) {////
                base.push_back(nd->vertex);
            }
        }

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


void removeChild_delete(Node* parent, Node* child) {
    int idx = -1;
    for (int i = 0; i < parent->child_count; ++i) {
        if (parent->childs[i] == child) {
            idx = i;
            break;
        }
    }
    assert(idx >= 0);

    free(child);

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


void prunePaths_delete(std::vector<SubtreeResult>& bothOut, Node* prefixTreeRoot) {
    for (auto& sr : bothOut) {
        auto& path = sr.path;

        for (int i = (int)path.size() - 1; i >= 0; --i) {
            Node* nd = path[i];

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

//            printf("the back: %d\n", path.back()->vertex);
            path.pop_back();
        }
    }
}


bool isSubsequence_delete(const std::vector<int>& seq, const std::vector<int>& subseq) {
    size_t i = 0, j = 0;
    while (i < seq.size() && j < subseq.size()) {
        if (seq[i] == subseq[j]) ++j;
        ++i;
    }
    return j == subseq.size();
}


void insertResultIntoEither_delete(
        const std::vector<std::vector<int>>& result,
        const std::vector<SubtreeResult>& eitherOut,
        Node* prefixTreeRoot)
{
    for (auto const& clique : result) {
        if (clique.empty()) continue;

       
        bool skip = false;
        for (auto const& sr : eitherOut) {

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

 
        int rootVal = clique[0];
        bool inserted = false;
        for (auto const& sr : eitherOut) {
            if (sr.root->vertex == rootVal) {
     
                insertIntoPrefixTree(sr.root, clique);
                inserted = true;
                break;
            }
        }


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
        return result; 
    }
    int first, second, weight; 
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

        auto result = generateAugmentedCliques_delete(bothOut, v1, v2);

        prunePaths_delete(bothOut, prefixTreeRoot);

        insertResultIntoEither_delete(result, eitherOut, prefixTreeRoot);
    }
}

void removeSubtree(Node* virtualRoot, Node* subtreeRoot) {
    int n = virtualRoot->child_count;
    for (int i = 0; i < n; ++i) {
        if (virtualRoot->childs[i] == subtreeRoot) {
  
            free_tree(subtreeRoot);
    
            virtualRoot->childs[i] = virtualRoot->childs[n - 1];

            virtualRoot->child_count--;
            return;
        }
    }
}


void updateTrees_forAdd(std::vector<SubtreeResult>& paths1,
                        std::vector<SubtreeResult>& paths2,
                        const std::vector<std::vector<int>>& result, Node* prefixTreeRoot) {

    for (const auto& newClique : result) {
//        for (int v : newClique) { printf("%d ", v);}
//        printf("\n");

        for (auto& subtreeRes : paths1) {
//            for (Node *v : subtreeRes.path) { printf("%d ", v->vertex);}
//            printf("\n");
            if (subtreeRes.root && pathContainedInNewClique(subtreeRes.path, newClique)) {
//                printf("1.remove\n");
//                for (Node *v : subtreeRes.path) { printf("%d %d ", v->vertex, v->child_count);}
//                printf("\n");
                //removeRedundantSuffix(subtreeRes, newClique);

                removeSubtree(prefixTreeRoot, subtreeRes.root);
                subtreeRes.root = nullptr;
                subtreeRes.path.clear();
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

                removeSubtree(prefixTreeRoot, subtreeRes.root);
                subtreeRes.root = nullptr;
                subtreeRes.path.clear();
            }
        }
//        printf("after remove:\n");
//        print_tree(prefixTreeRoot, 0);


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


void addEdgeMaintain_Tree(Node *prefixTreeRoot, const char* edgeFilePath) {
    std::string filePath(edgeFilePath);
    std::vector<std::pair<int, int>> addEdge = readPairsFromFile(filePath);
    for (auto e : addEdge) {
        int v1 = e.first;
        int v2 = e.second;

        //auto start1 = std::chrono::high_resolution_clock::now();
        auto [paths1, paths2] = findVertexSubtreePaths(prefixTreeRoot, v1, v2);
        //auto end1 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> duration1 = end1 - start1;
        //std::cout << "findVertexSubtreePaths execution time: " << duration1.count()  << " seconds" << std::endl;


//        int treeNode = 0;
//        for (const auto &item: paths1) {
//            treeNode += item.path.size();
//        }
//        for (const auto &item: paths2) {
//            treeNode += item.path.size();
//        }


        //auto start2 = std::chrono::high_resolution_clock::now();
        auto all_intersections = computeAllIntersections(paths1, paths2);

        auto result = filterNonContained(all_intersections);
        for (int i = 0; i < result.size(); ++i) {
            result[i].push_back(v1);
            result[i].push_back(v2);
            //treeNode += result[i].size();
            std::sort(result[i].begin(), result[i].end());
        }
        //std::cout << "the all used tree node number : " << treeNode << std::endl;

        updateTrees_forAdd(paths1, paths2, result, prefixTreeRoot);
        //auto end2 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> duration2 = end2 - start2;
        //std::cout << "Maintenance process execution time: " << duration2.count() / 1000000 << " seconds" << std::endl;
    }
}


void insertResultIntoEither_TreeDelete(const std::vector<std::vector<int>>& result, const std::vector<SubtreeResult>& eitherOut, Node* prefixTreeRoot) {
    for (auto const& clique : result) {
        if (clique.empty()) continue;


        bool skip = false;
        for (auto const& sr : eitherOut) {

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


        Node *current = prefixTreeRoot;
        for (int v : clique) {
            Node *newNode = create_node(v, false);
            add_child(current, newNode);
            current = newNode;
        }
        current->is_leaf = true;

    }
}


void deleteEdgeMaintain_Tree(Node *prefixTreeRoot, const char* edgeFilePath) {
    std::string filePath(edgeFilePath);
    std::vector<std::pair<int, int>> deleteEdges = readPairsFromFile(filePath);
    for (auto e : deleteEdges) {
        int v1 = e.first;
        int v2 = e.second;
        std::vector<SubtreeResult> bothOut;
        std::vector<SubtreeResult> eitherOut;


//        int treeNode = 0;
//        for (const auto &item: bothOut) {
//            treeNode += item.path.size();
//        }
//        for (const auto &item: eitherOut) {
//            treeNode += item.path.size();
//        }

        //auto start1 = std::chrono::high_resolution_clock::now();

        findBranchesForDelete(prefixTreeRoot, v1, v2, bothOut, eitherOut);
        //auto end1 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> duration1 = end1 - start1;
        //std::cout << "findBranchesForDelete execution time: " << duration1.count()  << " seconds" << std::endl;

        //auto start2 = std::chrono::high_resolution_clock::now();

        auto result = generateAugmentedCliques_delete(bothOut, v1, v2);

        for (int i = 0; i < result.size(); ++i) {
            //treeNode += result[i].size();
        }
        //std::cout << "the all used tree node number : " << treeNode << std::endl;

 
        for (auto sr : bothOut) {
            removeSubtree(prefixTreeRoot, sr.root);
        }

        insertResultIntoEither_TreeDelete(result, eitherOut, prefixTreeRoot);

        //auto end2 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> duration2 = end2 - start2;
        //std::cout << "Maintenance process execution time: " << duration2.count()  << " seconds" << std::endl;
    }
}