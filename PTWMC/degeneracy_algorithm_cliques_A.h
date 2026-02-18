//
// Created by 16374 on 2025/2/27.
//

#ifndef BICLIQUEINDEX1_DEGENERACY_ALGORITHM_CLIQUES_A_H
#define BICLIQUEINDEX1_DEGENERACY_ALGORITHM_CLIQUES_A_H

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <list>

#include"misc.h"
#include"LinkedList.h"
#include"MemoryManager.h"
#include"degeneracy_helper.h"
#include "bicliqueIndex.h"


//void listAllCliquesDegeneracyRecursive_A(double *,
//                                         int* vertexSets, int* vertexLookup,
//                                         int** neighborsInP, int* numNeighbors,
//                                         int beginX, int beginP, int beginR, int max_k, double *nCalls, double *sumP, double *sqP, int rsize, int drop);

// 修改后的递归函数（添加树节点参数）
void listAllCliquesDegeneracyRecursive_A(
        double * cliqueCounts,
        int* vertexSets, int* vertexLookup,
        int** neighborsInP, int* numNeighbors,
        int beginX, int beginP, int beginR, int max_k,
        double *nCalls, double *sumP, double *sqP,
        int rsize, int drop,
        Node* parent  // 新增：父节点指针
);

//void listAllCliquesDegeneracy_A(double * cliqueCounts, NeighborListArray** orderingArray,
//                                 int size, int max_k, double *nCalls, double *sumP, double *sqP, int L);
// 修改后的 listAllCliquesDegeneracy_A 函数
void listAllCliquesDegeneracy_A(double * cliqueCounts, NeighborListArray** orderingArray,
                                int size, int max_k, double *nCalls, double *sumP, double *sqP, int L, Node*** rootNodes, int* rootCount);

// 极大团索引的 listAllCliquesDegeneracy_A 函数
void listAllCliquesDegeneracy_A_maximal(double * cliqueCounts, NeighborListArray** orderingArray,
                                int size, int max_k, double *nCalls, double *sumP, double *sqP, Node*** rootNodes, int* rootCount);

void listAllMaximalCliquesDegeneracy(NeighborListArray** orderingArray, int size, Node* prefixTreeRoot);
inline void fillInPandXForRecursiveCallDegeneracy( int vertex, int orderNumber,
                                                   int* vertexSets, int* vertexLookup,
                                                   NeighborListArray** orderingArray,
                                                   int** neighborsInP, int* numNeighbors,
                                                   int* pBeginX, int *pBeginP, int *pBeginR,
                                                   int* pNewBeginX, int* pNewBeginP, int *pNewBeginR);
void listAllMaximalCliquesDegeneracyRecursive(long* cliqueCount,
                                              std::list<int> &partialClique,
                                              int* vertexSets, int* vertexLookup,
                                              int** neighborsInP, int* numNeighbors,
                                              int beginX, int beginP, int beginR, Node* root);
inline int findBestPivotNonNeighborsDegeneracy( int** pivotNonNeighbors, int* numNonNeighbors,
                                                int* vertexSets, int* vertexLookup,
                                                int** neighborsInP, int* numNeighbors,
                                                int beginX, int beginP, int beginR);
inline void moveToRDegeneracy( int vertex,
                               int* vertexSets, int* vertexLookup,
                               int** neighborsInP, int* numNeighbors,
                               int* pBeginX, int *pBeginP, int *pBeginR,
                               int* pNewBeginX, int* pNewBeginP, int *pNewBeginR);
inline void moveFromRToXDegeneracy( int vertex,
                                    int* vertexSets, int* vertexLookup,
                                    int* pBeginX, int* pBeginP, int* pBeginR );
#endif //BICLIQUEINDEX1_DEGENERACY_ALGORITHM_CLIQUES_A_H
