//
// Created by 16374 on 2025/2/27.
//
#include<limits.h>
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include <list>


#include"misc.h"
#include"LinkedList.h"
#include"MemoryManager.h"
#include"degeneracy_helper.h"
#include"degeneracy_algorithm_cliques_A.h"
#include "bicliqueIndex.h"
#include <windows.h> // 仅Windows需要
#include <algorithm>

void setConsoleToUTF8() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // UTF-8代码页
#endif
}
extern double nCr[1001][401];

/*! \brief 初始化数据结构，并调用递归函数 listAllCliquesDegeneracyRecursive_A 枚举所有团

    \param cliqueCount 用于存储每种大小的团的数量

    \param orderingArray 图的退化度顺序数组

    \param size 图的顶点数

    \param max_k 最大团上界（退化度）

    \param nCalls 递归调用次数

    \param sumP 中间结果的总和

    \param sqP 中间结果的平方和
*/
//void listAllCliquesDegeneracy_A(double * cliqueCounts, NeighborListArray** orderingArray,
//                                int size, int max_k, double *nCalls, double *sumP, double *sqP, int L)
void listAllCliquesDegeneracy_A(double * cliqueCounts, NeighborListArray** orderingArray,
                                int size, int max_k, double *nCalls, double *sumP, double *sqP, int L, Node*** rootNodes, int* rootCount)
{
    *nCalls = (*nCalls) + 1;

    // vertex sets are stored in an array like this:
    // |--X--|--P--|
    int* vertexSets = (int *)Calloc(size, sizeof(int));//存储顶点的集合

    // vertex i is stored in vertexSets[vertexLookup[i]]
    int* vertexLookup = (int *)Calloc(size, sizeof(int));//存储每个顶点在 vertexSets 中的位置

    int** neighborsInP = (int **)Calloc(size, sizeof(int*));//存储每个顶点在集合 P 中的邻居
    int* numNeighbors = (int *)Calloc(size, sizeof(int));//存储每个顶点在集合 P 中的邻居数量

    int i = 0;

    while(i<size)
    {
        vertexLookup[i] = i;
        vertexSets[i] = i;
        neighborsInP[i] = (int *)Calloc(1, sizeof(int));
        numNeighbors[i] = 1;
        i++;
    }

    int beginX = 0;// 集合 X 的起始位置
    int beginP = 0;// 集合 P 的起始位置
    int beginR = size;// 集合 R 的起始位置

    // 创建根节点列表
//    Node** rootNodes = (Node**)calloc(size, sizeof(Node*));
//    int rootCount = 0;

    // for each vertex
    for(i=0;i<size;i++)
    {
        int vertex = (int)orderingArray[i]->vertex;
        //printf("1.vertex: %d\n", vertex);
        //printf("1.beginR: %d\n", beginR);
        int newBeginX, newBeginP, newBeginR;

        ////如果vertex 和 orderingArray[i]->later中的顶点都为一侧，不对vertex进行递归，可以跳过循环
        // 新增判断逻辑：如果顶点和所有later邻居同侧则跳过

        int isLeft = (vertex < L); // 判断顶点所在侧
        int allSameSide = 1;
        //遍历所有later邻居
        if (orderingArray[i]->laterDegree > 0) {
            for(int j=0; j<orderingArray[i]->laterDegree; j++)
            {
                int laterVertex = orderingArray[i]->later[j];
                //printf("2.later: %d", laterVertex);
                // 检查邻居是否与当前顶点同侧
                if((isLeft && laterVertex >= L) || (!isLeft && laterVertex < L))
                {
                    allSameSide = 0;
                    break;
                }
            }
        }
        // 全部同侧则跳过递归
        if(allSameSide) {
            //printf("3.vertex: %d\n", vertex);
            //beginR = beginR + 1; // 保持beginR的递增逻辑同步
            //beginP = beginR;
            //beginX = beginR;//这里有问题
            continue;
        }



        // set P to be later neighbors and X to be earlier neighbors
        // of vertex
        fillInPandXForRecursiveCallDegeneracyCliques( i, vertex,
                                                      vertexSets, vertexLookup,
                                                      orderingArray,
                                                      neighborsInP, numNeighbors,
                                                      &beginX, &beginP, &beginR,
                                                      &newBeginX, &newBeginP, &newBeginR);

        *sumP = *sumP + (newBeginR - newBeginP);
        *sqP = *sqP + ((newBeginR - newBeginP)*(newBeginR - newBeginP));

        // recursively compute maximal cliques containing vertex, some of its
        // later neighbors, and avoiding earlier neighbors 递归计算包含当前顶点、其部分后续邻居，并排除先前邻居的极大团
        int drop = 0;
        int rsize = 1;
        //printf("1.vertex:%d\n", vertex);////这个vertex作为树的根节点

        // 为当前顶点创建根节点
        Node* root = create_node(vertex, false);
        (*rootNodes)[*rootCount] = root;
        (*rootCount)++;

//        listAllCliquesDegeneracyRecursive_A(cliqueCounts,
//                                            vertexSets, vertexLookup,
//                                            neighborsInP, numNeighbors,
//                                            newBeginX, newBeginP, newBeginR, max_k, nCalls, sumP, sqP, rsize, drop);
        listAllCliquesDegeneracyRecursive_A(cliqueCounts,
                                            vertexSets, vertexLookup,
                                            neighborsInP, numNeighbors,
                                            newBeginX, newBeginP, newBeginR, max_k, nCalls, sumP, sqP, rsize, drop, root  // 新增：父节点指针
        );


        beginR = beginR + 1;
        //printf("2.beginR: %d\n", beginR);
    }

    cliqueCounts[0] = 1;


    Free(vertexSets);
    Free(vertexLookup);

    for(i = 0; i<size; i++)
    {
        Free(neighborsInP[i]);
        Free(orderingArray[i]->later);
        Free(orderingArray[i]->earlier);
        Free(orderingArray[i]);
    }

    Free(neighborsInP);
    Free(numNeighbors);

    return;
}

//极大团索引的 listAllCliquesDegeneracy_A 函数
void listAllCliquesDegeneracy_A_maximal(double * cliqueCounts, NeighborListArray** orderingArray,
                                int size, int max_k, double *nCalls, double *sumP, double *sqP, Node*** rootNodes, int* rootCount)
{
    setConsoleToUTF8(); // 设置控制台编码
    *nCalls = (*nCalls) + 1;

    // vertex sets are stored in an array like this:
    // |--X--|--P--|
    int* vertexSets = (int *)Calloc(size, sizeof(int));//存储顶点的集合

    // vertex i is stored in vertexSets[vertexLookup[i]]
    int* vertexLookup = (int *)Calloc(size, sizeof(int));//存储每个顶点在 vertexSets 中的位置

    int** neighborsInP = (int **)Calloc(size, sizeof(int*));//存储每个顶点在集合 P 中的邻居
    int* numNeighbors = (int *)Calloc(size, sizeof(int));//存储每个顶点在集合 P 中的邻居数量

    int i = 0;

    while(i<size)
    {
        vertexLookup[i] = i;
        vertexSets[i] = i;
        neighborsInP[i] = (int *)Calloc(1, sizeof(int));
        numNeighbors[i] = 1;
        i++;
    }

    int beginX = 0;// 集合 X 的起始位置
    int beginP = 0;// 集合 P 的起始位置
    int beginR = size;// 集合 R 的起始位置


    // for each vertex
    for(i=0;i<size;i++)
    {
        int vertex = (int)orderingArray[i]->vertex;
        //int vertex = (int)orderingArray[size - i - 1]->vertex;////
        printf("1.vertex: %d\n", vertex);
        printf("1.beginR: %d\n", beginR);
        int newBeginX, newBeginP, newBeginR;


        // set P to be later neighbors and X to be earlier neighbors
        // of vertex
        ////要改
        fillInPandXForRecursiveCallDegeneracyCliques( i, vertex,
                                                      vertexSets, vertexLookup,
                                                      orderingArray,
                                                      neighborsInP, numNeighbors,
                                                      &beginX, &beginP, &beginR,
                                                      &newBeginX, &newBeginP, &newBeginR);

        *sumP = *sumP + (newBeginR - newBeginP);
        *sqP = *sqP + ((newBeginR - newBeginP)*(newBeginR - newBeginP));

        // 打印X和P集合
        printf("当前顶点 %d:\n", vertex);
        printf("R集合元素：");
        for (int j = beginR; j < 4; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nX集合元素：");
        for (int j = newBeginX; j < newBeginP; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nP集合元素：");
        for (int j = newBeginP; j < newBeginR; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\n\n");

        // recursively compute maximal cliques containing vertex, some of its
        // later neighbors, and avoiding earlier neighbors 递归计算包含当前顶点、其部分后续邻居，并排除先前邻居的极大团
        int drop = 0;
        int rsize = 1;
        printf("1.vertex:%d\n", vertex);////这个vertex作为树的根节点

        // 为当前顶点创建根节点
        Node* root = create_node(vertex, false);
        (*rootNodes)[*rootCount] = root;
        (*rootCount)++;

//        listAllCliquesDegeneracyRecursive_A(cliqueCounts,
//                                            vertexSets, vertexLookup,
//                                            neighborsInP, numNeighbors,
//                                            newBeginX, newBeginP, newBeginR, max_k, nCalls, sumP, sqP, rsize, drop);
        listAllCliquesDegeneracyRecursive_A(cliqueCounts,
                                            vertexSets, vertexLookup,
                                            neighborsInP, numNeighbors,
                                            newBeginX, newBeginP, newBeginR, max_k, nCalls, sumP, sqP, rsize, drop, root  // 新增：父节点指针
        );


        beginR = beginR + 1;
        //printf("2.beginR: %d\n", beginR);
    }

    cliqueCounts[0] = 1;


    Free(vertexSets);
    Free(vertexLookup);

    for(i = 0; i<size; i++)
    {
        Free(neighborsInP[i]);
        Free(orderingArray[i]->later);
        Free(orderingArray[i]->earlier);
        Free(orderingArray[i]);
    }

    Free(neighborsInP);
    Free(numNeighbors);

    return;
}

/*! \brief Recursively list all maximal cliques containing all of
           all vertices in R, some vertices in P and no vertices in X.递归地列出所有包含以下顶点的极大团：R 中的所有顶点，P 中的部分顶点，并且不包含 X 中的任何顶点

    \param cliqueCount A pointer to the number of maximal cliques computed
                       thus far.一个指向当前已计算的极大团数量的指针

    \param cliques A linked list of cliques to return. <b>(only available when compiled
                   with RETURN_CLIQUES_ONE_BY_ONE defined)</b> 用于返回的团的链表。<b>（仅在编译时定义了 RETURN_CLIQUES_ONE_BY_ONE 时可用）</b>

    \param partialClique A linked list storing R, the partial clique for this
                         recursive call. 存储 R 的链表，R 是当前递归调用中的部分团

    \param vertexSets An array containing sets of vertices divided into sets X, P, R and other.一个数组，包含划分为集合 X、P、R 和其他集合的顶点

    \param vertexLookup A lookup table indexed by vertex number, storing the index of that
                        vertex in vertexSets. 一个按顶点编号索引的查找表，存储该顶点在 vertexSets 中的索引

    \param neighborsInP Maps vertices to arrays of neighbors such that
                        neighbors in P fill the first cells 将顶点映射到邻居数组，使得在 P 中的邻居占据数组的前部  每个顶点在 P 中的邻居

    \param numNeighbors An the neighbor of neighbors a vertex had in P,
                        the first time this function is called, this bound is
                        used to keep us from allocating more than linear space. 顶点在 P 中的邻居数量，在首次调用此函数时，此限制用于确保我们不会分配超过线性空间的内存

    \param beginX The index where set X begins in vertexSets. 集合 X 在 vertexSets 中的起始索引

    \param beginP The index where set P begins in vertexSets. 集合 P 在 vertexSets 中的起始索引

    \param beginR The index where set R begins in vertexSets. 集合 R 在 vertexSets 中的起始索引

    \param nCalls 递归调用次数

    \param rsize 当前团的大小

    \param drop 表示当前递归步骤中需要“丢弃”的顶点数

*/
////
//void listAllCliquesDegeneracyRecursive_A( double * cliqueCounts,
//                                          int* vertexSets, int* vertexLookup,
//                                          int** neighborsInP, int* numNeighbors,
//                                          int beginX, int beginP, int beginR, int max_k,
//                                          double *nCalls, double *sumP, double *sqP,
//                                          int rsize, int drop)
// 修改后的递归函数实现
void listAllCliquesDegeneracyRecursive_A(
        double * cliqueCounts,
        int* vertexSets, int* vertexLookup,
        int** neighborsInP, int* numNeighbors,
        int beginX, int beginP, int beginR, int max_k,
        double *nCalls, double *sumP, double *sqP,
        int rsize, int drop,
        Node* parent  // 当前父节点
)
{
    //更新递归调用次数和中间结果
    *nCalls = (*nCalls) + 1;
    *sumP = *sumP + (beginR - beginP);
    *sqP = *sqP + ((beginR - beginP)*(beginR - beginP));

    //如果集合 P 为空或当前团大小超过 max_k，则终止递归
    //if ((beginP >= beginR) || (rsize-drop > max_k))
    ////
    if ((beginP >= beginR && beginX >= beginR) || (rsize-drop > max_k))
    {
        for (int i=drop; (i>=0) && (rsize-i <= max_k); i--)
        {
            int k = rsize - i;
            cliqueCounts[k] += nCr[drop][i];//使用组合数 nCr 更新团的数量 //表示从 drop 个元素中选取 i 个元素的组合数
        }
        parent->is_leaf = true;////

        // 打印R集合
        //printf("当前顶点 %d:\n", vertex);
        printf("R集合元素：");
        for (int j = beginR; j < 4; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\n");

        return;
    }

    int* myCandidatesToIterateThrough;
    int numCandidatesToIterateThrough = 0;

    // get the candidates to add to R to make a maximal clique 选择枢轴顶点，并获取候选顶点列表
    ////改
    int pivot = findBestPivotNonNeighborsDegeneracyCliques( &myCandidatesToIterateThrough,
                                                            &numCandidatesToIterateThrough,
                                                            vertexSets, vertexLookup,
                                                            neighborsInP, numNeighbors,
                                                            beginX, beginP, beginR);

    printf("2.pivot:%d\n", pivot);

    // add candidate vertices to the partial clique one at a time and
    // search for maximal cliques 遍历候选顶点，递归枚举包含该顶点的团，如果顶点是枢轴顶点，则增加 drop 的值。
    if(numCandidatesToIterateThrough != 0)////每个pivot作为1的vertex的子节点
    {
        int iterator = 0;
        while(iterator < numCandidatesToIterateThrough)
        {
            // vertex to be added to the partial clique
            int vertex = myCandidatesToIterateThrough[iterator];

            // 创建当前顶点节点
            Node* child = create_node(vertex, false);
            add_child(parent, child);
            printf("3.vertex:%d\n", vertex);////作为2的pivot的子节点

            int newBeginX, newBeginP, newBeginR;

            // add vertex into partialClique, representing R.

            // swap vertex into R and update all data structures // 将顶点移动到集合 R
            ////改
            moveToRDegeneracyCliques( vertex,
                                      vertexSets, vertexLookup,
                                      neighborsInP, numNeighbors,
                                      &beginX, &beginP, &beginR,
                                      &newBeginX, &newBeginP, &newBeginR);



            // recursively compute maximal cliques with new sets R, P and X // 递归枚举团
//            if (vertex == pivot)
//                listAllCliquesDegeneracyRecursive_A(cliqueCounts,
//                                                    vertexSets, vertexLookup,
//                                                    neighborsInP, numNeighbors,
//                                                    newBeginX, newBeginP, newBeginR, max_k, nCalls, sumP, sqP, rsize+1, drop+1);
//            else
//                listAllCliquesDegeneracyRecursive_A(cliqueCounts,
//                                                    vertexSets, vertexLookup,
//                                                    neighborsInP, numNeighbors,
//                                                    newBeginX, newBeginP, newBeginR, max_k, nCalls, sumP, sqP, rsize+1, drop);

            // 递归调用时传入新创建的节点作为父节点
            if (vertex == pivot) {
                listAllCliquesDegeneracyRecursive_A(
                        cliqueCounts, vertexSets, vertexLookup,
                        neighborsInP, numNeighbors,
                        newBeginX, newBeginP, newBeginR, max_k,
                        nCalls, sumP, sqP,
                        rsize+1, drop+1,
                        child  // 传入子节点作为新父节点
                );
            } else {
                listAllCliquesDegeneracyRecursive_A(
                        cliqueCounts, vertexSets, vertexLookup,
                        neighborsInP, numNeighbors,
                        newBeginX, newBeginP, newBeginR, max_k,
                        nCalls, sumP, sqP,
                        rsize+1, drop,
                        child  // 传入子节点作为新父节点
                );
            }

            // 将顶点从 R 移动到 X
            moveFromRToXDegeneracyCliques( vertex,
                                           vertexSets, vertexLookup,
                                           &beginX, &beginP, &beginR );

            iterator++;
        }

        // 打印当前递归层的X和P集合
        printf("1.递归调用（当前深度 %d）:\n", rsize);
        printf("R集合元素：");
        for (int j = beginR; j < 4; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nX集合元素（范围 %d-%d）:", beginX, beginP - 1);
        for (int j = beginX; j < beginP; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nP集合元素（范围 %d-%d）:", beginP, beginR - 1);
        for (int j = beginP; j < beginR; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\n\n");


        // swap vertices that were moved to X back into P, for higher recursive calls. 将候选顶点从集合 X 移回集合 P
        iterator = 0;
        while(iterator < numCandidatesToIterateThrough)
        {
            int vertex = myCandidatesToIterateThrough[iterator];
            int vertexLocation = vertexLookup[vertex];

            beginP--;
            vertexSets[vertexLocation] = vertexSets[beginP];
            vertexSets[beginP] = vertex;
            vertexLookup[vertex] = beginP;
            vertexLookup[vertexSets[vertexLocation]] = vertexLocation;

            iterator++;
        }

        // 打印当前递归层的X和P集合
        printf("2.递归调用（当前深度 %d）:\n", rsize);
        printf("R集合元素：");
        for (int j = beginR; j < 4; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nX集合元素（范围 %d-%d）:", beginX, beginP - 1);
        for (int j = beginX; j < beginP; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nP集合元素（范围 %d-%d）:", beginP, beginR - 1);
        for (int j = beginP; j < beginR; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\n\n");

    }

    // 标记叶子节点（当没有更多候选顶点时）
//    if(parent->child_count == 0) {
//        parent->is_leaf = true;
//    }

    // don't need to check for emptiness before freeing, since
    // something will always be there (we allocated enough memory
    // for all of P, which is nonempty)
    Free(myCandidatesToIterateThrough);

    return;
}

void listAllMaximalCliquesDegeneracy(NeighborListArray** orderingArray, int size, Node* prefixTreeRoot)
{
    // vertex sets are stored in an array like this:
    // |--X--|--P--|
    int* vertexSets = (int*)Calloc(size, sizeof(int));

    // vertex i is stored in vertexSets[vertexLookup[i]]
    int* vertexLookup = (int*)Calloc(size, sizeof(int));

    int** neighborsInP = (int**)Calloc(size, sizeof(int*));
    int* numNeighbors = (int*)Calloc(size, sizeof(int));

    // compute the degeneracy order
////    clock_t clockStart = clock();
    //NeighborListArray** orderingArray = computeDegeneracyOrderArray(adjList, size);
////    clock_t clockEnd = clock();
////    clock_t timeDegeneracyOrder = clockEnd - clockStart;

    int i = 0;

    while(i<size)
    {
        vertexLookup[i] = i;
        vertexSets[i] = i;
        neighborsInP[i] = (int*)Calloc(1, sizeof(int));
        numNeighbors[i] = 1;
        i++;
    }

    int beginX = 0;
    int beginP = 0;
    int beginR = size;

    long cliqueCount = 0;

    std::vector<int> partialClique;
    int maximal_clique = 0;
    // 创建根节点（虚拟根节点，vertex值可以设为-1）
    //Node* prefixTreeRoot = create_node(-1, false);

    // for each vertex
    for(i=0;i<size;i++)
    {
        int vertex = (int)orderingArray[i]->vertex;
        //printf("1.vertex: %d\n", vertex);

#ifdef PRINT_CLIQUES_TOMITA_STYLE
        printf("%d ", vertex);
#endif

        // add vertex to partial clique R
        partialClique.push_back(vertex);

        int newBeginX, newBeginP, newBeginR;

        // set P to be later neighbors and X to be earlier neighbors
        // of vertex
        fillInPandXForRecursiveCallDegeneracy( i, vertex,
                                               vertexSets, vertexLookup,
                                               orderingArray,
                                               neighborsInP, numNeighbors,
                                               &beginX, &beginP, &beginR,
                                               &newBeginX, &newBeginP, &newBeginR);
        //printf("1.vertex:%d\n", vertex);

        // recursively compute maximal cliques containing vertex, some of its
        // later neighbors, and avoiding earlier neighbors
        listAllMaximalCliquesDegeneracyRecursive(&cliqueCount,
                                                 partialClique,
                                                 vertexSets, vertexLookup,
                                                 neighborsInP, numNeighbors,
                                                 newBeginX, newBeginP, newBeginR, prefixTreeRoot, maximal_clique);

#ifdef PRINT_CLIQUES_TOMITA_STYLE
        printf("b ");
#endif

        beginR = beginR + 1;

        partialClique.pop_back();
    }

    printf("maximal clique number: %d\n", maximal_clique);

    //cerr << endl;
    //cerr << "Largest Difference  : " << largestDifference << endl;
    //cerr << "Num     Differences : " << numLargeJumps << endl;
    //cerr << "Time Computing Pivot: " << ((double)(timeComputingPivot)/(double)(CLOCKS_PER_SEC)) << endl;
    //cerr << "Time Moving R to X  : " << ((double)(timeMovingFromRtoX)/(double)(CLOCKS_PER_SEC)) << endl;
    //cerr << "Time Moving   to R  : " << ((double)(timeMovingToR)/(double)(CLOCKS_PER_SEC)) << endl;
    //cerr << "Time Moving X to P  : " << ((double)(timeMovingXToP)/(double)(CLOCKS_PER_SEC)) << endl;
    //cerr << "Time Making X and P : " << ((double)(timeFillInPX)/(double)(CLOCKS_PER_SEC)) << endl;
    //cerr << "Time Degeneracy Ordr: " << ((double)(timeDegeneracyOrder)/(double)(CLOCKS_PER_SEC)) << endl;

    partialClique.clear();

    Free(vertexSets);
    Free(vertexLookup);

    for(i = 0; i<size; i++)
    {
        Free(neighborsInP[i]);
        delete orderingArray[i];
    }

    Free(orderingArray);
    Free(neighborsInP);
    Free(numNeighbors);

    //return cliqueCount;
    return;
}
inline void fillInPandXForRecursiveCallDegeneracy( int vertex, int orderNumber,
                                                   int* vertexSets, int* vertexLookup,
                                                   NeighborListArray** orderingArray,
                                                   int** neighborsInP, int* numNeighbors,
                                                   int* pBeginX, int *pBeginP, int *pBeginR,
                                                   int* pNewBeginX, int* pNewBeginP, int *pNewBeginR)
{
////        clock_t startClock = clock();
    int vertexLocation = vertexLookup[vertex];

    (*pBeginR)--;
    vertexSets[vertexLocation] = vertexSets[*pBeginR];
    vertexLookup[vertexSets[*pBeginR]] = vertexLocation;
    vertexSets[*pBeginR] = vertex;
    vertexLookup[vertex] = *pBeginR;

    *pNewBeginR = *pBeginR;
    *pNewBeginP = *pBeginR;

    // swap later neighbors of vertex into P section of vertexSets
    int j = 0;
    while(j<orderingArray[orderNumber]->laterDegree)
    {
        int neighbor = orderingArray[orderNumber]->later[j];
        int neighborLocation = vertexLookup[neighbor];

        (*pNewBeginP)--;

        vertexSets[neighborLocation] = vertexSets[*pNewBeginP];
        vertexLookup[vertexSets[*pNewBeginP]] = neighborLocation;
        vertexSets[*pNewBeginP] = neighbor;
        vertexLookup[neighbor] = *pNewBeginP;

        j++;
    }

    *pNewBeginX = *pNewBeginP;

    // swap earlier neighbors of vertex into X section of vertexSets
    j = 0;
    while(j<orderingArray[orderNumber]->earlierDegree)
    {
        int neighbor = orderingArray[orderNumber]->earlier[j];
        int neighborLocation = vertexLookup[neighbor];

        (*pNewBeginX)--;
        vertexSets[neighborLocation] = vertexSets[*pNewBeginX];
        vertexLookup[vertexSets[*pNewBeginX]] = neighborLocation;
        vertexSets[*pNewBeginX] = neighbor;
        vertexLookup[neighbor] = *pNewBeginX;

        Free(neighborsInP[neighbor]);
        neighborsInP[neighbor] = (int*)Calloc(MIN(*pNewBeginR-*pNewBeginP,orderingArray[neighbor]->laterDegree), sizeof(int));
        numNeighbors[neighbor] = 0;

        // fill in NeighborsInP
        int k = 0;
        while(k<orderingArray[neighbor]->laterDegree)
        {
            int laterNeighbor = orderingArray[neighbor]->later[k];
            int laterNeighborLocation = vertexLookup[laterNeighbor];
            if(laterNeighborLocation >= *pNewBeginP && laterNeighborLocation < *pNewBeginR)
            {
                neighborsInP[neighbor][numNeighbors[neighbor]] = laterNeighbor;
                numNeighbors[neighbor]++;
            }

            k++;
        }

        j++;

    }

    // reset numNeighbors and neighborsInP for this vertex
    j = *pNewBeginP;
    while(j<*pNewBeginR)
    {
        int vertexInP = vertexSets[j];
        numNeighbors[vertexInP] = 0;
        Free(neighborsInP[vertexInP]);
        neighborsInP[vertexInP]=(int*)Calloc( MIN( *pNewBeginR-*pNewBeginP,
                                                   orderingArray[vertexInP]->laterDegree
                                                   + orderingArray[vertexInP]->earlierDegree), sizeof(int));

        j++;
    }

    // count neighbors in P, and fill in array of neighbors
    // in P
    j = *pNewBeginP;
    while(j<*pNewBeginR)
    {
        int vertexInP = vertexSets[j];

        int k = 0;
        while(k<orderingArray[vertexInP]->laterDegree)
        {
            int laterNeighbor = orderingArray[vertexInP]->later[k];
            int laterNeighborLocation = vertexLookup[laterNeighbor];

            if(laterNeighborLocation >= *pNewBeginP && laterNeighborLocation < *pNewBeginR)
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
////    clock_t endClock = clock();
////    timeFillInPX += (endClock - startClock);
}

void listAllMaximalCliquesDegeneracyRecursive(long* cliqueCount,
                                                                   std::vector<int> &partialClique,
                                                                   int* vertexSets, int* vertexLookup,
                                                                   int** neighborsInP, int* numNeighbors,
                                                                   int beginX, int beginP, int beginR, Node* root, int& maximal_clique)
{

    // if X is empty and P is empty, process partial clique as maximal
    if(beginX >= beginP && beginP >= beginR)
    {
        //partialClique.sort();//按顶点序号升序排列
        maximal_clique++;
        std::sort(partialClique.begin(), partialClique.end());
        //直接将每个极大团存为树
        Node *current = root;
        for (int v : partialClique) {
            Node *newNode = create_node(v, false);
            add_child(current, newNode);
            current = newNode;
        }
        current->is_leaf = true;

        // 插入到前缀树
        //insertIntoPFTree(root, partialClique);
        //打印极大团
//        for(auto a : partialClique) {
//            printf("%d ", a);
//        }
//        printf("\n");
        return;
    }

    // avoid work if P is empty.
    if(beginP >= beginR)
        return;

    int* myCandidatesToIterateThrough;
    int numCandidatesToIterateThrough;

    // get the candidates to add to R to make a maximal clique
    int pivot = findBestPivotNonNeighborsDegeneracy( &myCandidatesToIterateThrough,
                                         &numCandidatesToIterateThrough,
                                         vertexSets, vertexLookup,
                                         neighborsInP, numNeighbors,
                                         beginX, beginP, beginR);
    //printf("2.pivot:%d\n", pivot);

    // add candiate vertices to the partial clique one at a time and
    // search for maximal cliques
    if(numCandidatesToIterateThrough != 0)
    {
        int iterator = 0;
        while(iterator < numCandidatesToIterateThrough)
        {
            // vertex to be added to the partial clique
            int vertex = myCandidatesToIterateThrough[iterator];
            //printf("3.vertex:%d\n", vertex);

#ifdef PRINT_CLIQUES_TOMITA_STYLE
            printf("%d ", vertex);
#endif

            int newBeginX, newBeginP, newBeginR;

            // add vertex into partialClique, representing R.
            partialClique.push_back(vertex);

            // swap vertex into R and update all data structures
            moveToRDegeneracy( vertex,
                               vertexSets, vertexLookup,
                               neighborsInP, numNeighbors,
                               &beginX, &beginP, &beginR,
                               &newBeginX, &newBeginP, &newBeginR);

            // recursively compute maximal cliques with new sets R, P and X
            listAllMaximalCliquesDegeneracyRecursive(cliqueCount,
                                                     partialClique,
                                                     vertexSets, vertexLookup,
                                                     neighborsInP, numNeighbors,
                                                     newBeginX, newBeginP, newBeginR, root, maximal_clique);

#ifdef PRINT_CLIQUES_TOMITA_STYLE
            printf("b ");
#endif

            // remove vertex from partialClique
            partialClique.pop_back();

            moveFromRToXDegeneracy( vertex,
                                    vertexSets, vertexLookup,
                                    &beginX, &beginP, &beginR );

            iterator++;
        }

        // swap vertices that were moved to X back into P, for higher recursive calls.
        iterator = 0;

////    clock_t clockStart = clock();
        while(iterator < numCandidatesToIterateThrough)
        {
            int vertex = myCandidatesToIterateThrough[iterator];
            int vertexLocation = vertexLookup[vertex];

            beginP--;
            vertexSets[vertexLocation] = vertexSets[beginP];
            vertexSets[beginP] = vertex;
            vertexLookup[vertex] = beginP;
            vertexLookup[vertexSets[vertexLocation]] = vertexLocation;

            iterator++;
        }
////    clock_t clockEnd = clock();
////    timeMovingXToP += (clockEnd - clockStart);
    }

    // don't need to check for emptiness before freeing, since
    // something will always be there (we allocated enough memory
    // for all of P, which is nonempty)
    Free(myCandidatesToIterateThrough);
}

inline int findBestPivotNonNeighborsDegeneracy( int** pivotNonNeighbors, int* numNonNeighbors,
                                                int* vertexSets, int* vertexLookup,
                                                int** neighborsInP, int* numNeighbors,
                                                int beginX, int beginP, int beginR)
{
////    clock_t clockStart = clock();
    int pivot = -1;
    int maxIntersectionSize = -1;

    // iterate over each vertex in P union X
    // to find the vertex with the most neighbors in P.
    int j = beginX;
    while(j<beginR)
    {
        int vertex = vertexSets[j];
        int numPotentialNeighbors = MIN(beginR - beginP, numNeighbors[vertex]);

        int numNeighborsInP = 0;

        int k = 0;
        while(k<numPotentialNeighbors)
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
    // computation of non-neighbors.
    *pivotNonNeighbors = (int*)Calloc(beginR-beginP, sizeof(int));
    memcpy(*pivotNonNeighbors, &vertexSets[beginP], (beginR-beginP)*sizeof(int));

    // we will decrement numNonNeighbors as we find neighbors
    *numNonNeighbors = beginR-beginP;

    int numPivotNeighbors = MIN(beginR - beginP, numNeighbors[pivot]);

    // mark the neighbors of pivot that are in P.
    j = 0;
    while(j<numPivotNeighbors)
    {
        int neighbor = neighborsInP[pivot][j];
        int neighborLocation = vertexLookup[neighbor];

        if(neighborLocation >= beginP && neighborLocation < beginR)
        {
            (*pivotNonNeighbors)[neighborLocation-beginP] = -1;
        }
        else
        {
            break;
        }

        j++;
    }

    // move non-neighbors of pivot in P to the beginning of
    // pivotNonNeighbors and set numNonNeighbors appriopriately.

    // if a vertex is marked as a neighbor, the we move it
    // to the end of pivotNonNeighbors and decrement numNonNeighbors.
    j = 0;
    while(j<*numNonNeighbors)
    {
        int vertex = (*pivotNonNeighbors)[j];

        if(vertex == -1)
        {
            (*numNonNeighbors)--;
            (*pivotNonNeighbors)[j] = (*pivotNonNeighbors)[*numNonNeighbors];
            continue;
        }

        j++;
    }

////    clock_t clockEnd = clock();
////
////    timeComputingPivot += (clockEnd - clockStart);

    return pivot;
}

inline void moveToRDegeneracy( int vertex,
                               int* vertexSets, int* vertexLookup,
                               int** neighborsInP, int* numNeighbors,
                               int* pBeginX, int *pBeginP, int *pBeginR,
                               int* pNewBeginX, int* pNewBeginP, int *pNewBeginR)
{

////    clock_t clockStart = clock();
    int vertexLocation = vertexLookup[vertex];

    (*pBeginR)--;
    vertexSets[vertexLocation] = vertexSets[*pBeginR];
    vertexLookup[vertexSets[*pBeginR]] = vertexLocation;
    vertexSets[*pBeginR] = vertex;
    vertexLookup[vertex] = *pBeginR;

    // this is not a typo, initially newX is empty
    *pNewBeginX = *pBeginP;
    *pNewBeginP = *pBeginP;
    *pNewBeginR = *pBeginP;

    int sizeOfP = *pBeginR - *pBeginP;

    int j = *pBeginX;
    while(j<*pNewBeginX)
    {
        int neighbor = vertexSets[j];
        int neighborLocation = j;

        int incrementJ = 1;

        int numPotentialNeighbors = MIN(sizeOfP, numNeighbors[neighbor]);

        int k = 0;
        while(k<numPotentialNeighbors)
        {
            if(neighborsInP[neighbor][k] == vertex)
            {
                (*pNewBeginX)--;
                vertexSets[neighborLocation] = vertexSets[(*pNewBeginX)];
                vertexLookup[vertexSets[(*pNewBeginX)]] = neighborLocation;
                vertexSets[(*pNewBeginX)] = neighbor;
                vertexLookup[neighbor] = (*pNewBeginX);
                incrementJ=0;
            }

            k++;
        }

        if(incrementJ) j++;
    }

    j = (*pBeginP);
    while(j<(*pBeginR))
    {
        int neighbor = vertexSets[j];
        int neighborLocation = j;

        int numPotentialNeighbors = MIN(sizeOfP, numNeighbors[neighbor]);

        int k = 0;
        while(k<numPotentialNeighbors)
        {
            if(neighborsInP[neighbor][k] == vertex)
            {
                vertexSets[neighborLocation] = vertexSets[(*pNewBeginR)];
                vertexLookup[vertexSets[(*pNewBeginR)]] = neighborLocation;
                vertexSets[(*pNewBeginR)] = neighbor;
                vertexLookup[neighbor] = (*pNewBeginR);
                (*pNewBeginR)++;
            }

            k++;
        }

        j++;
    }

    j = (*pNewBeginX);

    while(j < *pNewBeginR)
    {
        int thisVertex = vertexSets[j];

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

////    clock_t clockEnd = clock();
////
////    timeMovingToR += (clockEnd - clockStart);
}

inline void moveFromRToXDegeneracy( int vertex,
                                    int* vertexSets, int* vertexLookup,
                                    int* pBeginX, int* pBeginP, int* pBeginR )
{
////    clock_t clockStart = clock();
    int vertexLocation = vertexLookup[vertex];

    //swap vertex into X and increment beginP and beginR
    vertexSets[vertexLocation] = vertexSets[*pBeginP];
    vertexLookup[vertexSets[*pBeginP]] = vertexLocation;
    vertexSets[*pBeginP] = vertex;
    vertexLookup[vertex] = *pBeginP;

    *pBeginP = *pBeginP + 1;
    *pBeginR = *pBeginR + 1;

////    clock_t clockEnd = clock();
////
////    timeMovingFromRtoX += (clockEnd - clockStart);
}