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

    /*
    if (argc != 9)
    {
        printf("Incorrect number of arguments.\n");
        printf("./degeneracy_cliques -i <file_path> -t <type> -k <max_clique_size> -d <data_flag>\n");
        printf("file_path: path to file\n");
        printf("type: A/V/E. A for just k-clique information, V for per-vertex k-cliques, E for per-edge k-cliques\n");
        printf("max_clique_size: max_clique_size. If 0, calculate for all k.\n");
        printf("data_flag: 1 if information is to be output to a file, 0 otherwise.\n");
        return 0;
    }
    */

    //int L;
    //int R;
    int n; // number of vertices
    int m; // 2x number of edges

    // char *opt = NULL;
    //int opt;
    // 从命令行参数获取文件路径
    //char* fpath = argv[1]; // 第一个参数是文件路径
    char *fpath = (char *)Calloc(1000, sizeof(char));
    char t;
    int flag_d;
    int max_k = 0;

    /*
    while((opt = getopt(argc, argv, ":i:t:k:d:")) != -1)
    {
        switch(opt)
        {
            case 'i':
                // printf("In case i. optarg = %s\n", optarg);
                // strcpy(fpath, optarg);
                fpath = optarg;
                // printf("fpath = %s\n", fpath);
                break;
            case 't':
                t = *optarg;
                if ((t != 'A') && (t != 'V') && (t != 'E'))
                {
                    printf("Incorrect type. Type should be A, V or E.\n");
                    return 0;
                }
                break;
            case 'k':
                max_k = atoi(optarg);
                break;
            case 'd':
                flag_d = atoi(optarg);
                if ((flag_d < 0) || (flag_d > 2))
                {
                    printf("Incorrect flag for data. Shoudld be 0, 1 or 2.\n");
                    return 0;
                }
                break;
            default:
                printf("In default case.\n");
                abort ();
        }
    }
    */
    fpath = "D:\\school\\second year\\First Semester\\biclique index\\datasets\\uai.txt";//amazon0601 //email-Enron  youtube
    t = 'A';
    max_k = 0;
    flag_d = 0;

    //std::vector<float> vertexWeight;//存储顶点的权重
    //LinkedList** biadjacencyList = readBipartiteGraph(&L, &R, &n, &m ,fpath, vertexWeight);

    //printf("1=====================");
    ////有问题
    //convertBipartiteToNormalGraph(biadjacencyList, L, n);
    //普通图极大团索引
    //存储边权重
    std::unordered_map<std::pair<int, int>, float, pair_hash> edgeWeight;
    printf("read graph.\n");
    LinkedList** adjacencyList = readInGraphAdjListToDoubleEdges_ew(&n, &m, fpath, edgeWeight);
//    double sumdeg = 0;
//    for (int i = 0; i < n; i++) {
//        sumdeg += length(adjacencyList[i]);
//    }
    printf("average degree = %.2f\n", m * 1.0 /  (n * 2));

    // 示例：打印邻接表验证
//    for (int i = 0; i < n; i++) {
//        printf("vertex %d ", i);
//        Link* curr = adjacencyList[i]->head->next;
//        while (curr != adjacencyList[i]->tail) {
//            printf("%d ", curr->data);
//            //printf("%f ", edgeWeight[std::make_pair(curr->data, i)]);
//            if (i < curr->data) {
//                printf("%f ", edgeWeight[std::make_pair(i, curr->data)]);
//            }
//            else{
//                printf("%f ", edgeWeight[std::make_pair(curr->data, i)]);
//            }
//            curr = curr->next;
//        }
//        printf("\n");
//    }


    int i;
    populate_nCr();
    printf("about to call runAndPrint.\n");
    char *gname = "";

    runAndPrintStatsCliques_maximal(adjacencyList, n, gname, t, max_k, flag_d);



    /*i = 0;
    while(i<n)
    {
        destroyLinkedList(adjacencyList[i]);
        i++;
    }

    Free(adjacencyList);*/



    //printf("2=====================");
    //LinkedList** adjacencyList = readInGraphAdjListToDoubleEdges(&n, &m, fpath);



    // 示例：打印邻接表验证
//    for (int i = 0; i < n; i++) {
//        printf("Vertex %d (side: %s): ", i, i < L ? "LEFT" : "RIGHT");
//        Link* curr = biadjacencyList[i]->head->next;
//        while (curr != biadjacencyList[i]->tail) {
//            printf("%d ", curr->data);
//            curr = curr->next;
//        }
//        printf(";%f", vertexWeight[i]);
//        printf("\n");
//    }




    /*
    char *gname = basename(fpath);
    //printf("=====");
    char *lastdot = strrchr (gname, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    */
    /*
    int i;
    populate_nCr();
    printf("about to call runAndPrint.\n");
    char *gname = "";
    ////有问题
    runAndPrintStatsCliques(biadjacencyList, n, gname, t, max_k, flag_d, L, vertexWeight);



    i = 0;
    while(i<n)
    {
        destroyLinkedList(biadjacencyList[i]);
        i++;
    }

    Free(biadjacencyList);
     */

    return 0;
}
