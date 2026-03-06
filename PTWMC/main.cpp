#include <iostream>

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<limits.h>
#include<unistd.h>
#include<libgen.h>
#include <vector>
#include <unordered_map>

#include"misc.h"
#include"LinkedList.h"
#include"MemoryManager.h"

/*
struct pair_hash
{
    template<class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};*/

int main(int argc,char** argv)
{

    int n; // number of vertices
    int m; // 2x number of edges

    if (argc != 4)
    {
        printf("Usage:\n");
        printf("./bIndex.out <graph_file_path> <add|delete|none> <edge_update_file_path>\n");
        printf("Example:\n");
        printf("./bIndex.out graph.txt add add_edges.txt\n");
        printf("./bIndex.out graph.txt delete delete_edges.txt\n");
        printf("./bIndex.out graph.txt none _\n");
        return 0;
    }


    char* fpath = argv[1];             
    const char* updateMode = argv[2];   // "add" / "delete" / "none"
    const char* edgeUpdatePath = argv[3]; 
    //char *fpath = (char *)Calloc(1000, sizeof(char));
    char t;
    int flag_d;
    int max_k = 0;
    t = 'A';
    max_k = 0;
    flag_d = 0;

    std::unordered_map<std::pair<int, int>, float, pair_hash> edgeWeight;
    std::vector<float> vertexWeight;
    printf("read graph.\n");
    LinkedList** adjacencyList = readInGraphAdjListToDoubleEdges_ew(&n, &m, fpath, edgeWeight, vertexWeight);

    printf("average degree = %.2f\n", m * 1.0 /  (n * 2));


    int i;

    printf("about to call runAndPrint.\n");
    char *gname = "";

    runAndPrintStatsCliques_maximal(adjacencyList, n, gname, t, max_k, flag_d, updateMode, edgeUpdatePath);

    return 0;
}
