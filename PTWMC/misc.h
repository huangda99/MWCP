//
// Created by 16374 on 2025/2/27.
//

#ifndef BICLIQUEINDEX1_MISC_H
#define BICLIQUEINDEX1_MISC_H

#include"LinkedList.h"
#include"degeneracy_helper.h"
#include"degeneracy_algorithm_cliques_A.h"
#include <unordered_map>
//#include"degeneracy_algorithm_cliques_V.h"
//#include"degeneracy_algorithm_cliques_E.h"

#define MAX(x,y) (x > y? x:y)
#define MIN(x,y) (x < y? x:y)
#define MAX_CSIZE 400
struct pair_hash
{
    template<class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};
void populate_nCr();

int nodeComparator(void* node1, void* node2);

void printArray(int* array, int size);

void printArrayOfLinkedLists(LinkedList** listOfLists, int size);

void printInt(void* integer);

void destroyCliqueResults(LinkedList* cliques);

LinkedList** readInGraphAdjList(int* n, int* m);

LinkedList** readInGraphAdjListToDoubleEdges(int* n, int* m, char *fpath);
LinkedList** readInGraphAdjListToDoubleEdges_ew(int* n, int* m, char *fpath, std::unordered_map<std::pair<int, int>, float, pair_hash> &edgeWeight);



void runAndPrintStatsCliques(LinkedList** adjListLinked,
                             int n, const char * gname,
                             char T, int max_k, int flag_d, int L, std::vector<float> &vertexWeight);

//极大团索引
void runAndPrintStatsCliques_maximal(LinkedList** adjListLinked,
                                     int n, const char * gname,
                                     char T, int max_k, int flag_d,
                                     const char* updateMode,
                                     const char* edgeUpdatePath);


int findNbrCSC(int u, int v, int *CSCindex, int *CSCedges);

void moveFromRToXDegeneracyCliques( int vertex,
                                    int* vertexSets, int* vertexLookup,
                                    int* pBeginX, int* pBeginP, int* pBeginR);

void moveToRDegeneracyCliques( int vertex,
                               int* vertexSets, int* vertexLookup,
                               int** neighborsInP, int* numNeighbors,
                               int* pBeginX, int *pBeginP, int *pBeginR,
                               int* pNewBeginX, int* pNewBeginP, int *pNewBeginR);

void fillInPandXForRecursiveCallDegeneracyCliques( int vertex, int orderNumber,
                                                   int* vertexSets, int* vertexLookup,
                                                   NeighborListArray** orderingArray,
                                                   int** neighborsInP, int* numNeighbors,
                                                   int* pBeginX, int *pBeginP, int *pBeginR,
                                                   int* pNewBeginX, int* pNewBeginP, int *pNewBeginR);

int findBestPivotNonNeighborsDegeneracyCliques( int** pivotNonNeighbors, int* numNonNeighbors,
                                                int* vertexSets, int* vertexLookup,
                                                int** neighborsInP, int* numNeighbors,
                                                int beginX, int beginP, int beginR);

#endif //BICLIQUEINDEX1_MISC_H
