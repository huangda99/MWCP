//
// Created by 16374 on 2025/2/27.
//
#include<assert.h>
#include<stdio.h>
#include<time.h>
#include <chrono>
//#include<sys/resource.h>
#include<stdlib.h>
#include<string.h>
#include <unordered_map>
#include <algorithm>


#include"misc.h"
#include"LinkedList.h"
#include"MemoryManager.h"
#include"degeneracy_helper.h"
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

double nCr[1001][401];

void populate_nCr()
{
    FILE *infile;
    infile = fopen("D:\\school\\second year\\First Semester\\biclique index\\bicliqueIndex1\\nCr.txt","r");
    double d=0;
    if(infile==NULL)
    {
        printf("file could not be opened\n");
        exit(1);
    }


    for(int row = 0; row < 1001; ++row)
    {
        for (int col = 0; col < 401; ++col)
        {
            if (!fscanf(infile,"%lf,",&d))
                fprintf(stderr, "Error\n");
            // fprintf(stderr, "%lf\n", d);
            nCr[row][col] = d;
        }
    }
    fclose(infile);
}

/*! \brief compare integers return -1,0,1 for <,=,>

    \param node1 an integer

    \param node2 an integer

    \return -1 if <, 0 if =, and 1 if >.
*/

int nodeComparator(int node1, int node2)
{
    if ((int)(size_t)node1 < (int)(size_t)node2)
        return -1;
    if((int)(size_t)node1 > (int)(size_t)node2)
        return 1;

    return 0;
}

/*! \brief compare integer pointers; return -1,0,1 for <,=,>;
           used for calling sort().

    \param node1 a pointer to an integer

    \param node2 a pointer to an integer

    \return -1 if <, 0 if =, and 1 if >.
*/

int sortComparator(int node1, int node2)
{
    if (*(int*)node1 < *(int*)node2)
        return -1;
    if(*(int*)node1 > *(int*)node2)
        return 1;

    return 0;
}

int qsortComparator(const void * node1, const void * node2)
{
    return ( *(int*)node1 - *(int*)node2 );
}

/*! \brief print an array of integers to standard out.

    \param array the array to print

    \param size the length of the array
*/

void printArray(int* array, int size)
{
    int i = 0;
    while(i<size)
        printf("%d ", array[i++]);
    printf("\n");
}

/*! \brief print an abbreviated version of an adjacency list

    \param listOfLists the adjacency list

    \param size the number of vertices in the graph
*/

void printArrayOfLinkedLists(LinkedList** listOfLists, int size)
{
    // list graph contents

    int i=0;

    while(i<size)
    {
        if(!isEmpty(listOfLists[i]))
        {
            printf("%d:", i);
            // printListAbbv(listOfLists[i], &printInt);
        }
        i++;
    }
}

/*! \brief print a clique, that is formatted as an integer
           array ending with -1.

    \param clique the clique.
*/

void printClique(int* clique)
{
    int i = 0;
    while(clique[i]!=-1)
    {
        printf("%d", clique[i]);
        if(clique[i+1]!=-1)
            printf(" ");
        i++;
    }
    printf("\n");
}

/*! \brief print an integer

    \param integer an integer cast to a int
*/

void printInt(int integer)
{
    printf("%d", (int)(size_t)integer);
}

/*! \brief destroy a linked list of integer arrays that have
           -1 in the last cell, have have been allocated by
           the user.

    \param cliques the linked list of integer arrays
*/

void destroyCliqueResults(LinkedList* cliques)
{
    Link* curr = cliques->head->next;
    while(!isTail(curr))
    {
        int* clique = (int*)curr->data;

#ifdef DEBUG
        int i=0;
        while(clique[i] != -1)
        {
            printf("%d", clique[i]);
            if(clique[i+1] != -1)
                printf(" ");
            i++;
        }
        printf("\n");
#endif
        Free(clique);
        curr = curr->next;
    }

    destroyLinkedList(cliques);
}

/*! \brief read in a graph from stdin and return an
           adjacency list, as an array of linked lists
           of integers.

    \param n this will be the number of vertices in the
             graph when this function returns.

    \param m this will be 2x the number of edges in the
             graph when this function returns.

    \return an array of linked lists of integers (adjacency list)
            representation of the graph
*/
//从标准输入读取图的邻接表表示，适用于有向图
LinkedList** readInGraphAdjList(int* n, int* m)
{
    int u, v; // endvertices, to read edges.

    if(scanf("%d", n)!=1)
    {
        fprintf(stderr, "problem with line 1 in input file\n");
        exit(1);
    }

    if(scanf("%d", m)!=1)
    {
        fprintf(stderr, "problem with line 2 in input file\n");
        exit(1);
    }

#ifdef DEBUG
    printf("Number of vertices: %d\n", *n);
    printf("Number of edges: %d\n", *m);
#endif

    LinkedList** adjList = (LinkedList**)Calloc(*n, sizeof(LinkedList*));

    int i = 0;
    while(i < *n)
    {
        adjList[i] = createLinkedList();
        i++;
    }

    i = 0;

    while(i < *m)
    {
        if(scanf("%d,%d", &u, &v)!=2)
        {
            printf("problem with line %d in input file\n", i+2);
            exit(1);
        }
        assert(u < *n && u > -1);
        assert(v < *n && v > -1);
        if(u==v)
            printf("%d=%d\n", u, v);
        assert(u != v);

        addLast(adjList[u], (int)v);
        // addLast(adjList[v], (int)u);

        i++;
    }

#ifdef DEBUG
    printArrayOfLinkedLists(adjList, *n);
#endif

    return adjList;
}

/*! \brief read in a graph from stdin and return an
           adjacency list, as an array of linked lists
           of integers.

    \param n this will be the number of vertices in the
             graph when this function returns.

    \param m this will be 2x the number of edges in the
             graph when this function returns.

    \return an array of linked lists of integers (adjacency list)
            representation of the graph
*/
//从文件中读取图的邻接表表示，并将每条边存储两次，适用于无向图
LinkedList** readInGraphAdjListToDoubleEdges(int* n, int* m, char *fpath)
{
    int u, v; // endvertices, to read edges.

    FILE *fp;
    fp = fopen (fpath,"r");
    if (!fp)
    {
        fprintf(stderr, "Could not open input file.\n");
        exit(1);
    }

    //printf("=====");

    if(fscanf(fp, "%d %d", n, m)!=2)
    {
        fprintf(stderr, "Number of vertices: %d\n", *n);
        fprintf(stderr, "Number of edges: %d\n", *m);
        fprintf(stderr, "problem with line 1 in input file\n");
        exit(1);
    }
    LinkedList** adjList = (LinkedList**)Calloc(*n, sizeof(LinkedList*));

    int i = 0;
    //printf("=====");
    while(i < *n)
    {
        adjList[i] = createLinkedList();
        i++;
    }
    //printf("=====");
    i = 0;
    // double maxv = 0;
    while(i < *m)
    {
        if (fscanf(fp, "%d %d\n", &u, &v)!=2)
        {
            printf("problem with line %d in input file, u=%d, v=%d\n", i+2, u, v);
            exit(1);
        }
        // if ((u>= *n) || (v >= *n)) printf("u = %d, v = %d \n", u, v);
        // if ((double) u > maxv) maxv = (double) u;
        // if ((double) v > maxv) maxv = (double) v;
        assert(u < *n && u > -1);
        assert(v < *n && v > -1);
        assert(u != v);

        addLast(adjList[u], (int)v);
        addLast(adjList[v], (int)u);

        i++;
    }
    //printf("maxv = %lf \n", maxv);
    *m = (*m) * 2;
    //printf("=====");
    fclose(fp);
    return adjList;
}

//从文件中读取图的邻接表表示，并将每条边存储两次，适用于无向图  图的边有权重
LinkedList** readInGraphAdjListToDoubleEdges_ew(int* n, int* m, char *fpath, std::unordered_map<std::pair<int, int>, float, pair_hash> &edgeWeight)
{
    int u, v; // endvertices, to read edges.

    FILE *fp;
    fp = fopen (fpath,"r");
    if (!fp)
    {
        fprintf(stderr, "Could not open input file.\n");
        exit(1);
    }

    //printf("=====");

    if(fscanf(fp, "%d %d", n, m)!=2)
    {
        fprintf(stderr, "Number of vertices: %d\n", *n);
        fprintf(stderr, "Number of edges: %d\n", *m);
        fprintf(stderr, "problem with line 1 in input file\n");
        exit(1);
    }
    printf("vertices: %d, edges: %d\n", *n, *m);
    LinkedList** adjList = (LinkedList**)Calloc(*n, sizeof(LinkedList*));
    //edgeWeight.reserve(*m);
    int i = 0;
    //printf("=====");
    while(i < *n)
    {
        adjList[i] = createLinkedList();
        i++;
    }
    //printf("=====");
    i = 0;
    float w;
    // double maxv = 0;
    while(i < *m)
    {
        if (fscanf(fp, "%d %d %f\n", &u, &v, &w)!=3)
        {
            printf("problem with line %d in input file, u=%d, v=%d\n", i+2, u, v);
            exit(1);
        }
        // if ((u>= *n) || (v >= *n)) printf("u = %d, v = %d \n", u, v);
        // if ((double) u > maxv) maxv = (double) u;
        // if ((double) v > maxv) maxv = (double) v;
        assert(u < *n && u > -1);
        assert(v < *n && v > -1);
//        if (u == v){
//            printf("%d ,%d ", u, v);
//        }
        assert(u != v);
        std::pair<int, int> edge = std::make_pair(u, v);
        ////问题：这样存储边权重太慢了
        ////edgeWeight.insert({edge, w});
//        printf("w:%f ", w);
//        printf("ew:%f ",edgeWeight[edge]);
//        printf("\n");
        addLast(adjList[u], (int)v);
        addLast(adjList[v], (int)u);

        i++;
    }

//    for (auto e : edgeWeight) {
//        printf("%f ", edgeWeight[e.first]);
//    }

    //printf("maxv = %lf \n", maxv);
    *m = (*m) * 2;
    //printf("=====");
    fclose(fp);
    return adjList;
}

//核心函数，运行团枚举算法并打印统计信息
void runAndPrintStatsCliques(  LinkedList** adjListLinked,
                               int n, const char * gname,
                               char T, int max_k, int flag_d, int L, std::vector<float> &vertexWeight)
{
    //printf("In runAndPrint function.\n");
    fflush(stderr);
    int max_k_in = max_k;

    clock_t start = clock();

    double totalCliques = 0;
    int deg = 0, m = 0;
    FILE *fp;

    //printf("Before if of flag_d.\n");
    fflush(stdout);
    if ((flag_d == 1) || (flag_d == 2))
    {

        char *fname = (char *)Calloc(1000, sizeof(char));

        strcpy(fname, "results/");
        strcat(fname, gname);
        char * s_max_k = (char *)Calloc(10,sizeof(char));
        sprintf(s_max_k, "%d", max_k);
        if (max_k > 0) {strcat(fname, "_"); strcat(fname, s_max_k);}
        if (T == 'A')
        {
            if (flag_d == 2) strcat(fname, "_A_stat.txt");
            else strcat(fname, "_A.txt");
        }
        else if (T == 'V')
        {
            if (flag_d == 2) strcat(fname, "_V_stat.txt");
            else strcat(fname, "_V.txt");
        }
        else
        {
            if (flag_d == 2) strcat(fname, "_E_stat.txt");
            else strcat(fname, "_E.txt");
        }

        fp = fopen (fname,"w");
        if (!fp) printf("Could not open output file.\n");
    }
    //printf("Before computeDegeneracy.\n");
    fflush(stdout);

    //计算图的退化度顺序
    NeighborListArray** orderingArray = computeDegeneracyOrderArray(adjListLinked, n);
    //printf("Before for. After computeDegeneracy.\n");
    fflush(stdout);
    for (int i=0; i<n; i++)
    {
        if (deg < orderingArray[i]->laterDegree) deg = orderingArray[i]->laterDegree;
        m += orderingArray[i]->laterDegree;//m?
    }

    if (max_k == 0) max_k = deg + 1;

    if (T == 'A')
    {

        double *nCalls = (double *)Calloc(1, sizeof(double));
        double *sumP = (double *)Calloc(1, sizeof(double));
        double *sqP = (double *)Calloc(1, sizeof(double));

        double *cliqueCounts = (double *) Calloc((max_k)+1, sizeof(double));
        int rootCount = 0;//树索引数量
        Node** rootNodes = (Node**)calloc(n, sizeof(Node*));//树索引根节点
        listAllCliquesDegeneracy_A(cliqueCounts, orderingArray, n, max_k, nCalls, sumP, sqP, L, &rootNodes, &rootCount);
        //listAllCliquesDegeneracy_A(cliqueCounts, orderingArray, n, max_k, nCalls, sumP, sqP, L);
        clock_t end = clock();

        printf("time,nCalls,sumP,sqP,max_k,degen\n");
        printf("%lf,%lf,%lf,%lf,%d,%d\n\n", (double)(end-start)/(double)(CLOCKS_PER_SEC),*nCalls,*sumP,*sqP,max_k_in,deg);
        if ((flag_d == 1) || (flag_d == 2))
        {
            fprintf(fp, "time,nCalls,sumP,sqP,max_k,degen\n");
            fprintf(fp, "%lf,%lf,%lf,%lf,%d,%d\n\n", (double)(end-start)/(double)(CLOCKS_PER_SEC),*nCalls,*sumP,*sqP,max_k_in,deg);
        }

        if (flag_d == 1) fprintf(fp, "k, Ck\n");

        for (int i=0; i<=max_k; i++)
        {
            if (cliqueCounts[i] != 0)
            {
                printf("%d, %lf\n", i, cliqueCounts[i]);
                if (flag_d == 1) fprintf(fp, "%d, %lf\n", i, cliqueCounts[i]);
                totalCliques += cliqueCounts[i];
            }
        }

        printf("\n%lf total cliques\n", totalCliques);

        // 打印所有树
        printf("========print the biclique index========\n");
//        for(int i=0; i < rootCount; i++) {
//            print_tree(rootNodes[i], 0);
//        }


        printf("======biclique index finished======\n");
        analyzeMemory(rootNodes, rootCount);
        float maxWeight = 0.0;
        node* maxWeightBiclique = nullptr;
        // 记录开始时间
        auto start1 = std::chrono::high_resolution_clock::now();
        //查询所有树，找到顶点权重最大的二分团
        for(int i=0; i < rootCount; i++) {
            float weight = addWeightForBiclique(rootNodes[i], vertexWeight);
            if (maxWeight < weight) {
                maxWeight = weight;
                maxWeightBiclique = rootNodes[i];
            }
        }
        // 记录结束时间
        auto end1 = std::chrono::high_resolution_clock::now();
        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
        double elapsedTime = duration.count() / 1000000.0;  // 转换为秒
        printf("maxWeight= %f , searchtime= %lf s\n", maxWeight, elapsedTime);
        printf("========the vertex weighted maximum biclique:========\n");
        print_tree(maxWeightBiclique, 0);

        if (flag_d == 1) fprintf(fp, "\n%lf total cliques\n", totalCliques);

        Free(cliqueCounts);

        //释放树索引内存
        for(int i=0; i < rootCount; i++) {
            free_tree(rootNodes[i]);
        }
    }

    if (flag_d >= 1) fclose(fp);
    Free(orderingArray);

}

//极大团索引
void runAndPrintStatsCliques_maximal(LinkedList** adjListLinked,
                                     int n, const char * gname,
                                     char T, int max_k, int flag_d)
{
    printf("In runAndPrint function.\n");
    fflush(stderr);
    int max_k_in = max_k;

    clock_t start = clock();

    double totalCliques = 0;
    int deg = 0, m = 0;

    //计算图的退化度顺序
    NeighborListArray** orderingArray = computeDegeneracyOrderArray(adjListLinked, n);
    //printf("Before for. After computeDegeneracy.\n");
    fflush(stdout);
    for (int i=0; i<n; i++)
    {
        if (deg < orderingArray[i]->laterDegree) deg = orderingArray[i]->laterDegree;
        m += orderingArray[i]->laterDegree;//m?
    }

    if (max_k == 0) max_k = deg + 1;

    if (T == 'A')
    {

        double *nCalls = (double *)Calloc(1, sizeof(double));
        double *sumP = (double *)Calloc(1, sizeof(double));
        double *sqP = (double *)Calloc(1, sizeof(double));

        double *cliqueCounts = (double *) Calloc((max_k)+1, sizeof(double));
        //int rootCount = 0;//树索引数量
        //Node** rootNodes = (Node**)calloc(n, sizeof(Node*));//树索引根节点
        //listAllCliquesDegeneracy_A_maximal(cliqueCounts, orderingArray, n, max_k, nCalls, sumP, sqP, &rootNodes, &rootCount);

        // 创建前缀树根节点（虚拟根节点，vertex值设为-1）
        Node* prefixTreeRoot = create_node(-1, false);
        listAllMaximalCliquesDegeneracy(orderingArray, n, prefixTreeRoot);////枚举极大团
        ////clock_t end = clock();

        ////printf("time,nCalls,sumP,sqP,max_k,degen\n");
        ////printf("%lf,%lf,%lf,%lf,%d,%d\n\n", (double)(end-start)/(double)(CLOCKS_PER_SEC),*nCalls,*sumP,*sqP,max_k_in,deg);


//        for (int i=0; i<=max_k; i++)
//        {
//            if (cliqueCounts[i] != 0)
//            {
//                printf("%d, %lf\n", i, cliqueCounts[i]);
//                //if (flag_d == 1) fprintf(fp, "%d, %lf\n", i, cliqueCounts[i]);
//                totalCliques += cliqueCounts[i];
//            }
//        }
//
//        printf("\n%lf total cliques\n", totalCliques);

        // 打印所有树
        //printf("========print the maximal clique index========\n");
//        for(int i=0; i < rootCount; i++) {
//            print_tree(rootCount[i], 0);
//        }
        //print_tree(prefixTreeRoot, 0);
        ////将普通树索引转化成前缀树
        ////convert_to_prefix_tree(prefixTreeRoot);

        clock_t end = clock();

        printf("time,nCalls,sumP,sqP,max_k,degen\n");
        printf("%lf,%lf,%lf,%lf,%d,%d\n\n", (double)(end-start)/(double)(CLOCKS_PER_SEC),*nCalls,*sumP,*sqP,max_k_in,deg);
        printf("======clique index finished======\n");
        // 分析前缀树索引内存
        analyzePrefixTreeMemory(prefixTreeRoot);
        // 释放前缀树索引内存
        //freePrefixTree(prefixTreeRoot);

        //打印前缀树
        //print_tree(prefixTreeRoot, 0);

        //添加100条边维护总时间
        clock_t add_start = clock();
        //前缀树
        //addEdgeMaintain(prefixTreeRoot);
        //普通树
        /*addEdgeMaintain_Tree(prefixTreeRoot);
        clock_t add_end = clock();
        printf("the add maintain time: %lf\n", (double)(add_end-add_start)/(double)(CLOCKS_PER_SEC));
        printf("======after add edge clique index finished======\n");
        // 分析前缀树索引内存
        analyzePrefixTreeMemory(prefixTreeRoot);
        // 释放前缀树索引内存
        freePrefixTree(prefixTreeRoot);*/


        //删除100条边维护总时间
        clock_t delete_start = clock();
        //前缀树
        //deleteEdgeMaintain(prefixTreeRoot);
        //普通树
        deleteEdgeMaintain_Tree(prefixTreeRoot);
        clock_t delete_end = clock();
        printf("the delete maintain time: %lf\n", (double)(delete_end-delete_start)/(double)(CLOCKS_PER_SEC));
        printf("======after delete edge clique index finished======\n");
        // 分析前缀树索引内存
        analyzePrefixTreeMemory(prefixTreeRoot);
        // 释放前缀树索引内存
        freePrefixTree(prefixTreeRoot);

        //删除边的维护
        /*int v0 = 0;
        int v1 = 2;
        std::vector<SubtreeResult> bothOut;
        std::vector<SubtreeResult> eitherOut;
        findBranchesForDelete(prefixTreeRoot, v0, v1, bothOut, eitherOut);
        // 输出结果
//        printf("bothOut: \n");
//        for (auto& path : bothOut) {
//            printf("root: %d ", path.root->vertex);
//            for (Node *v : path.path) printf("%d ", v->vertex);
//            printf("\n");
//        }
//        printf("eitherOut: \n");
//        for (auto& path : eitherOut) {
//            printf("root: %d ", path.root->vertex);
//            for (Node *v : path.path) printf("%d ", v->vertex);
//            printf("\n");
//        }

        // 1) 生成新 cliques
        auto result = generateAugmentedCliques_delete(bothOut, v0, v1);
        // 打印结果
//        printf("result: \n");
//        for (const auto& vec : result) {
//            for (int x : vec) {
//                printf("%d ", x);
//            }
//            printf("\n");
//        }

        // 2) 删除原 bothOut 路径上的冗余节点
        //prunePaths_delete(bothOut, prefixTreeRoot);
        //(2) 普通树
        for (auto sr : bothOut) {
            removeSubtree(prefixTreeRoot, sr.root);
        }
//        //print_tree(prefixTreeRoot, 0);
//        // 3) 将新 cliques 插入到 eitherOut 中
        //insertResultIntoEither_delete(result, eitherOut, prefixTreeRoot);
        //(3)普通树, 将新 cliques 插入到 eitherOut 中
        insertResultIntoEither_TreeDelete(result, eitherOut, prefixTreeRoot);
        printf("======after delete clique index finished======\n");
        print_tree(prefixTreeRoot, 0);
        // 分析前缀树索引内存
        analyzePrefixTreeMemory(prefixTreeRoot);
        // 释放前缀树索引内存
        freePrefixTree(prefixTreeRoot);*/



        //插入后维护过程
        /*int v1 = 1;
        int v2 = 3;
        clock_t add_start = clock();
        // 查找顶点1和2的完整子树路径
        auto [paths1, paths2] = findVertexSubtreePaths(prefixTreeRoot, v1, v2);

        // 输出结果
        printf("vertex1: \n");
        for (auto& path : paths1) {
            printf("root: %d ", path.root->vertex);
            for (Node *v : path.path) printf("%d ", v->vertex);
            printf("\n");
        }
        printf("vertex2: \n");
        for (auto& path : paths2) {
            printf("root: %d ", path.root->vertex);
            for (Node *v : path.path) printf("%d ", v->vertex);
            printf("\n");
        }

        // 计算所有交集
        auto all_intersections = computeAllIntersections(paths1, paths2);
        // 找到新增的极大团
        auto result = filterNonContained(all_intersections);
        // 打印结果
        printf("result: \n");
        for (const auto& vec : result) {
            for (int x : vec) {
                printf("%d ", x);
            }
            printf("\n");
        }
        for (int i = 0; i < result.size(); ++i) {
            result[i].push_back(v1);
            result[i].push_back(v2);
            std::sort(result[i].begin(), result[i].end());
        }
        // 打印结果
        printf("result after sort: \n");
        for (const auto& vec : result) {
            for (int x : vec) {
                printf("%d ", x);
            }
            printf("\n");
        }
        ////现在的result就是新增的极大团了，需要把它插入到原来2个顶点相关的前缀树中并维持结构。
        // 先删除冗余树序列，再将新极大团插入，维持前缀树结构
        //updatePrefixTrees_forAdd(paths1, paths2, result, prefixTreeRoot);
        // 先删除冗余树序列，再将新极大团插入
        updateTrees_forAdd(paths1, paths2, result, prefixTreeRoot);
        clock_t add_end = clock();
        printf("the add maintain time: %lf\n", (double)(add_end-add_start)/(double)(CLOCKS_PER_SEC));

        print_tree(prefixTreeRoot, 0);

        printf("======after add clique index finished======\n");
        // 分析前缀树索引内存
        analyzePrefixTreeMemory(prefixTreeRoot);
        // 释放前缀树索引内存
        freePrefixTree(prefixTreeRoot);*/

        //统计树索引内存
        //analyzeMemory(rootNodes, rootCount);
        //float maxWeight = 0.0;
        //node* maxWeightBiclique = nullptr;
        // 记录开始时间
        //auto start1 = std::chrono::high_resolution_clock::now();
        //查询所有树，找到顶点权重最大的二分团
//        for(int i=0; i < rootCount; i++) {
//            float weight = addWeightForBiclique(rootNodes[i], vertexWeight);
//            if (maxWeight < weight) {
//                maxWeight = weight;
//                maxWeightBiclique = rootNodes[i];
//            }
//        }
        // 记录结束时间
        //auto end1 = std::chrono::high_resolution_clock::now();
        // 计算时间差
        //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
        //double elapsedTime = duration.count() / 1000000.0;  // 转换为秒
        //printf("maxWeight= %f , searchtime= %lf s\n", maxWeight, elapsedTime);
        //printf("========the vertex weighted maximum biclique:========\n");
        //print_tree(maxWeightBiclique, 0);

        //if (flag_d == 1) fprintf(fp, "\n%lf total cliques\n", totalCliques);

        Free(cliqueCounts);

        //释放树索引内存
//        for(int i=0; i < rootCount; i++) {
//            free_tree(rootNodes[i]);
//        }
    }

    Free(orderingArray);
    int i = 0;
    while(i<n)
    {
        destroyLinkedList(adjListLinked[i]);
        i++;
    }

    Free(adjListLinked);

}
/*! \brief Computes the vertex v in P union X that has the most neighbors in P,
           and places P \ {neighborhood of v} in an array. These are the
           vertices to consider adding to the partial clique during the current
           recursive call of the algorithm. 在集合 P 和 X 中找到最佳枢轴顶点（即在集合 P 中拥有最多邻居的顶点）。计算该枢轴顶点的非邻居顶点，并将它们存储在 pivotNonNeighbors 中

    \param pivotNonNeighbors  An initially unallocated pointer, which will contain the set
                              P \ {neighborhood of v} when this function completes. 用于存储枢轴顶点的非邻居顶点

    \param numNonNeighbors A pointer to a single integer, which has been preallocated,
                           which will contain the number of elements in pivotNonNeighbors. 用于存储非邻居顶点的数量

    \param vertexSets An array containing sets of vertices divided into sets X, P, R, and other.

    \param vertexLookup A lookup table indexed by vertex number, storing the index of that
                        vertex in vertexSets.

    \param neighborsInP Maps vertices to arrays of neighbors such that
                        neighbors in P fill the first cells

    \param numNeighbors An the neighbor of neighbors a vertex had in P,
                        the first time this function is called, this bound is
                        used to keep us from allocating more than linear space.

    \param beginX The index where set X begins in vertexSets.

    \param beginP The index where set P begins in vertexSets.

    \param beginR The index where set R begins in vertexSets.

*/
//找到最佳枢轴顶点及其非邻居
int findBestPivotNonNeighborsDegeneracyCliques( int** pivotNonNeighbors, int* numNonNeighbors,
                                                int* vertexSets, int* vertexLookup,
                                                int** neighborsInP, int* numNeighbors,
                                                int beginX, int beginP, int beginR)
{
    int pivot = -1;//最佳枢轴顶点
    int maxIntersectionSize = -1;//枢轴顶点在集合 P 中的最大邻居数量

    // iterate over each vertex in P union X  遍历集合 P 和 X 计算每个顶点在集合 P 中的邻居数量，选择在集合 P 中拥有最多邻居的顶点作为枢轴顶点
    // to find the vertex with the most neighbors in P.
    int j = beginP;
    while(j < beginR)
    {
        int vertex = vertexSets[j];
        int numPotentialNeighbors = MIN(beginR - beginP, numNeighbors[vertex]); //bug resolved by Shweta

        int numNeighborsInP = 0;

        int k = 0;
        while(k < numPotentialNeighbors)
        {
            int neighbor = neighborsInP[vertex][k];
            int neighborLocation = vertexLookup[neighbor];

            if(neighborLocation >= beginP && neighborLocation < beginR)
            {
                numNeighborsInP++;
            }
            else
            {
                break;
            }

            k++;
        }

        if(numNeighborsInP > maxIntersectionSize)
        {
            pivot = vertex;
            maxIntersectionSize = numNeighborsInP;
        }

        j++;
    }

    // compute non neighbors of pivot by marking its neighbors
    // and moving non-marked vertices into pivotNonNeighbors.
    // we must do this because this is an efficient way
    // to compute non-neighbors of a vertex in
    // an adjacency list.

    // we initialize enough space for all of P; this is
    // slightly space inefficient, but it results in faster
    // computation of non-neighbors.   计算枢轴顶点的非邻居
    *pivotNonNeighbors = (int *)Calloc(beginR-beginP, sizeof(int));  //初始化 pivotNonNeighbors 数组，存储pivot的所有非邻居顶点
    memcpy(*pivotNonNeighbors, &vertexSets[beginP], (beginR-beginP)*sizeof(int));

    // we will decrement numNonNeighbors as we find neighbors 当我们找到邻居时，会递减 numNonNeighbors
    *numNonNeighbors = beginR-beginP;

    int numPivotNeighbors = MIN(beginR - beginP, numNeighbors[pivot]); //bug resolved by Shweta

    // mark the neighbors of pivot that are in P.
    j = 0;
    while(j < numPivotNeighbors)
    {
        int neighbor = neighborsInP[pivot][j];
        int neighborLocation = vertexLookup[neighbor];

        if(neighborLocation >= beginP && neighborLocation < beginR)
        {
            (*pivotNonNeighbors)[neighborLocation-beginP] = -1;//标记枢轴顶点的邻居为 -1
        }
        else
        {
            break;
        }

        j++;
    }

    // move non-neighbors of pivot in P to the beginning of  将 P 中 pivot 的非邻居移动到 pivotNonNeighbors 的开头，并相应地设置 numNonNeighbors
    // pivotNonNeighbors and set numNonNeighbors appropriately.

    // if a vertex is marked as a neighbor, then we move it   如果一个顶点被标记为邻居，则将其移动到 pivotNonNeighbors 的末尾，并递减 numNonNeighbors
    // to the end of pivotNonNeighbors and decrement numNonNeighbors.
    j = 0;
    while(j < *numNonNeighbors)//？
    {
        int vertex = (*pivotNonNeighbors)[j];

        if(vertex == -1)
        {
            (*numNonNeighbors)--;
            (*pivotNonNeighbors)[j] = (*pivotNonNeighbors)[*numNonNeighbors];//将标记为 -1 的邻居顶点移动到数组末尾
            continue;
        }

        j++;
    }

    return pivot;
}

/*! \brief Move vertex to R, set P to vertex's later neighbors and
           set X to vertex's earlier neighbors.

    \param vertex The vertex to move to R.

    \param orderNumber The position of vertex in the ordering.

    \param vertexSets An array containing sets of vertices divided into sets X, P, and other.

    \param vertexLookup A lookup table indexed by vertex number, storing the index of that
                        vertex in vertexSets.

    \param orderingArray A degeneracy order of the input graph.

    \param neighborsInP Maps vertices to arrays of neighbors such that
                        neighbors in P fill the first cells 每个顶点在集合 P 中的邻居

    \param numNeighbors An the neighbor of neighbors a vertex had in P,
                        the first time this function is called, this bound is
                        used to keep us from allocating more than linear space. 每个顶点在集合 P 中的邻居数量

    \param pBeginX The index where set X begins in vertexSets. 集合 X 的起始位置

    \param pBeginP The index where set P begins in vertexSets.

    \param pBeginR The index where set R begins in vertexSets.

    \param pNewBeginX After function, contains the new index where set X begins
                      in vertexSets after adding vertex to R. 更新后的集合 X 的起始位置

    \param pNewBeginP After function, contains the new index where set P begins
                      in vertexSets after adding vertex to P.

    \param pNewBeginR After function, contains the new index where set R begins
                      in vertexSets after adding vertex to R.
*/
//在团枚举算法中将一个顶点移动到集合 R，并更新集合 P 和 X
//将指定的顶点移动到集合 R。
//将顶点的后续邻居（在退化度顺序中排在其后的邻居）移动到集合 P。
//将顶点的前驱邻居（在退化度顺序中排在其前的邻居）移动到集合 X。
//更新相关的数据结构和邻居信息。
void fillInPandXForRecursiveCallDegeneracyCliques( int vertex, int orderNumber,
                                                   int* vertexSets, int* vertexLookup,
                                                   NeighborListArray** orderingArray,
                                                   int** neighborsInP, int* numNeighbors,
                                                   int* pBeginX, int *pBeginP, int *pBeginR,
                                                   int* pNewBeginX, int* pNewBeginP, int *pNewBeginR)
{
    int vertexLocation = vertexLookup[vertex];//获取顶点在 vertexSets 中的位置 vertexLocation

    (*pBeginR)--;
    vertexSets[vertexLocation] = vertexSets[*pBeginR];//将顶点移动到集合 R 的起始位置，并更新 vertexLookup
    vertexLookup[vertexSets[*pBeginR]] = vertexLocation;
    vertexSets[*pBeginR] = vertex;
    vertexLookup[vertex] = *pBeginR;

    //新集合 R 和 P 的起始位置初始化为当前集合 R 的起始位置
    *pNewBeginR = *pBeginR;//pNewBeginR = *pBeginR;
    *pNewBeginP = *pBeginR;

    //printf("Before 1st while\n");
    // swap later neighbors of vertex into P section of vertexSets 将顶点的后续邻居移动到集合 P
    int j = 0;
    while( j < orderingArray[orderNumber]->laterDegree)
    {
        int neighbor = orderingArray[orderNumber]->later[j];//遍历顶点的后续邻居
        int neighborLocation = vertexLookup[neighbor];

        (*pNewBeginP)--;
        //将每个后续邻居移动到集合 P 的起始位置，并更新 vertexLookup
        vertexSets[neighborLocation] = vertexSets[*pNewBeginP];
        vertexLookup[vertexSets[*pNewBeginP]] = neighborLocation;
        vertexSets[*pNewBeginP] = neighbor;
        vertexLookup[neighbor] = *pNewBeginP;

        j++;
    }

    *pNewBeginX = *pNewBeginP;//新集合 X 的起始位置设置为新集合 P 的起始位置
//
    // reset numNeighbors and neighborsInP for this vertex 重置 neighborsInP 和 numNeighbors
    j = *pNewBeginP;
    //printf("Before 2nd while\n");
    //遍历集合 P 中的每个顶点，重置 numNeighbors 为 0。释放旧的 neighborsInP 内存，并重新分配内存
    while(j < *pNewBeginR)
    {
        int vertexInP = vertexSets[j];
        //printf("vertexInP = %d, numNeighbors[vertexInP]=%d\n", vertexInP, numNeighbors[vertexInP] );
        //printf("Address being freed: %p\n", neighborsInP[vertexInP]);
        numNeighbors[vertexInP] = 0;
        Free(neighborsInP[vertexInP]);
        //printf("Allocating %d space for neighborsInP[vertexInP].\n", min( *pNewBeginR-*pNewBeginP,
        //  orderingArray[vertexInP]->laterDegree
        //+ orderingArray[vertexInP]->earlierDegree));
        neighborsInP[vertexInP]= (int *)Calloc( MIN( *pNewBeginR-*pNewBeginP,
                                                     orderingArray[vertexInP]->laterDegree
                                                     + orderingArray[vertexInP]->earlierDegree), sizeof(int));//根据当前P的大小重新分配内存

        j++;
    }

    // count neighbors in P, and fill in array of neighbors 计算集合 P 中的邻居
    // in P 重新计算P集合内顶点间的邻接关系，确保后续递归步骤能快速访问有效邻居
    //遍历集合 P 中的每个顶点。遍历每个顶点的后续邻居。如果邻居也在集合 P 中，则更新 neighborsInP 和 numNeighbors。
    j = *pNewBeginP;
    //printf("Before 3rd while\n");
    while(j < *pNewBeginR)
    {
        int vertexInP = vertexSets[j];

        int k = 0;
        while(k < orderingArray[vertexInP]->laterDegree)
        {
            int laterNeighbor = orderingArray[vertexInP]->later[k];
            int laterNeighborLocation = vertexLookup[laterNeighbor];

            if(laterNeighborLocation >= *pNewBeginP && laterNeighborLocation < *pNewBeginR)//如果邻居也在P中，则互相记录
            {
                neighborsInP[vertexInP][numNeighbors[vertexInP]] = laterNeighbor;
                numNeighbors[vertexInP]++;
                neighborsInP[laterNeighbor][numNeighbors[laterNeighbor]] = vertexInP;
                numNeighbors[laterNeighbor]++;
            }

            k++;
        }

        j++;
    }
}



/*! \brief Move a vertex to the set R, and update sets P and X
           and the arrays of neighbors in P

    \param vertex The vertex to move to R. 要移动到集合 R 的顶点

    \param vertexSets An array containing sets of vertices divided into sets X, P, R, and other.

    \param vertexLookup A lookup table indexed by vertex number, storing the index of that
                        vertex in vertexSets.

    \param neighborsInP Maps vertices to arrays of neighbors such that
                        neighbors in P fill the first cells

    \param numNeighbors An the neighbor of neighbors a vertex had in P,
                        the first time this function is called, this bound is
                        used to keep us from allocating more than linear space.

    \param pBeginX The index where set X begins in vertexSets.

    \param pBeginP The index where set P begins in vertexSets.

    \param pBeginR The index where set R begins in vertexSets.

    \param pNewBeginX After function, contains the new index where set X begins
                      in vertexSets after adding vertex to R.

    \param pNewBeginP After function, contains the new index where set P begins
                      in vertexSets after adding vertex to P.

    \param pNewBeginR After function, contains the new index where set R begins
                      in vertexSets after adding vertex to R.
*/
//将顶点移动到集合 R，并更新集合 P 和 X
void moveToRDegeneracyCliques( int vertex,
                               int* vertexSets, int* vertexLookup,
                               int** neighborsInP, int* numNeighbors,
                               int* pBeginX, int *pBeginP, int *pBeginR,
                               int* pNewBeginX, int* pNewBeginP, int *pNewBeginR)
{

    int vertexLocation = vertexLookup[vertex];
    //将顶点移动到集合 R 的起始位置，并更新 vertexLookup
    (*pBeginR)--;//将集合 R 的起始指针 pBeginR 前移一位（扩展 R）
    vertexSets[vertexLocation] = vertexSets[*pBeginR];
    vertexLookup[vertexSets[*pBeginR]] = vertexLocation;
    vertexSets[*pBeginR] = vertex;
    vertexLookup[vertex] = *pBeginR;

    // this is not a typo, initially newX is empty
    *pNewBeginX = *pBeginP;
    *pNewBeginP = *pBeginP;
    *pNewBeginR = *pBeginP;

    int sizeOfP = *pBeginR - *pBeginP;
    //遍历集合 P 中的每个顶点，如果顶点是 vertex 的邻居，则将其移动到新集合 R 的起始位置  遍历原集合 P，移动受影响的邻居到新 R
    int j = (*pBeginP);
    while(j < (*pBeginR))
    {
        int neighbor = vertexSets[j];
        int neighborLocation = j;

        int numPotentialNeighbors = MIN(sizeOfP, numNeighbors[neighbor]);
        int k = 0;
        while(k < numPotentialNeighbors)
        {
            if(neighborsInP[neighbor][k] == vertex)
            {
                //将当前邻居顶点移动到新R区域
                vertexSets[neighborLocation] = vertexSets[(*pNewBeginR)];
                vertexLookup[vertexSets[(*pNewBeginR)]] = neighborLocation;
                vertexSets[(*pNewBeginR)] = neighbor;
                vertexLookup[neighbor] = (*pNewBeginR);
                (*pNewBeginR)++;// 扩展新R区域
            }

            k++;
        }

        j++;
    }

    j = (*pNewBeginP);
    //更新集合 P 中每个顶点的邻居列表
    while(j < *pNewBeginR)
    {
        int thisVertex = vertexSets[j];// 当前顶点

        int numPotentialNeighbors = MIN(sizeOfP, numNeighbors[thisVertex]);

        int numNeighborsInP = 0;

        int k = 0;
        while(k < numPotentialNeighbors)
        {
            int neighbor = neighborsInP[thisVertex][k];
            int neighborLocation = vertexLookup[neighbor];
            if(neighborLocation >= *pNewBeginP && neighborLocation < *pNewBeginR)
            {
                neighborsInP[thisVertex][k] = neighborsInP[thisVertex][numNeighborsInP];
                neighborsInP[thisVertex][numNeighborsInP] = neighbor;
                numNeighborsInP++;
            }
            k++;
        }

        j++;
    }
}

/*! \brief Move a vertex from the set R to the set X, and update all necessary pointers
           and arrays of neighbors in P

    \param vertex The vertex to move from R to X. 要移动的顶点

    \param vertexSets An array containing sets of vertices divided into sets X, P, R, and other.

    \param vertexLookup A lookup table indexed by vertex number, storing the index of that
                        vertex in vertexSets.

    \param pBeginX The index where set X begins in vertexSets.

    \param pBeginP The index where set P begins in vertexSets.

    \param pBeginR The index where set R begins in vertexSets.

*/
//将顶点从集合 R 移动到集合 X
void moveFromRToXDegeneracyCliques( int vertex,
                                    int* vertexSets, int* vertexLookup,
                                    int* pBeginX, int* pBeginP, int* pBeginR )
{
    int vertexLocation = vertexLookup[vertex];//获取顶点在 vertexSets 中的位置 vertexLocation

    //swap vertex into X and increment beginP and beginR 将顶点移动到集合 X 的起始位置，并更新 vertexLookup
    vertexSets[vertexLocation] = vertexSets[*pBeginP];
    vertexLookup[vertexSets[*pBeginP]] = vertexLocation;
    vertexSets[*pBeginP] = vertex;
    vertexLookup[vertex] = *pBeginP;

    *pBeginP = *pBeginP + 1;
    *pBeginR = *pBeginR + 1;
}

int findNbrCSC(int u, int v, int *CSCindex, int *CSCedges)
{
    int index = -1;

    int first = CSCindex[u], last = CSCindex[u+1] - 1;
    int middle = (first+last)/2;

    while (first <= last)
    {
        if (CSCedges[middle] < v)
            first = middle + 1;
        else if (CSCedges[middle] == v)
        {
            index = middle;
            break;
        }
        else
            last = middle - 1;

        middle = (first + last)/2;
    }

    return index;
}