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

void setConsoleToUTF8() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif
}


//void listAllCliquesDegeneracy_A(double * cliqueCounts, NeighborListArray** orderingArray,
//                                int size, int max_k, double *nCalls, double *sumP, double *sqP, int L)
void listAllCliquesDegeneracy_A(double * cliqueCounts, NeighborListArray** orderingArray,
                                int size, int max_k, double *nCalls, double *sumP, double *sqP, int L, Node*** rootNodes, int* rootCount)
{
    *nCalls = (*nCalls) + 1;

    // vertex sets are stored in an array like this:
    // |--X--|--P--|
    int* vertexSets = (int *)Calloc(size, sizeof(int));

    // vertex i is stored in vertexSets[vertexLookup[i]]
    int* vertexLookup = (int *)Calloc(size, sizeof(int));

    int** neighborsInP = (int **)Calloc(size, sizeof(int*));
    int* numNeighbors = (int *)Calloc(size, sizeof(int));

    int i = 0;

    while(i<size)
    {
        vertexLookup[i] = i;
        vertexSets[i] = i;
        neighborsInP[i] = (int *)Calloc(1, sizeof(int));
        numNeighbors[i] = 1;
        i++;
    }

    int beginX = 0;
    int beginP = 0;
    int beginR = size;

//    Node** rootNodes = (Node**)calloc(size, sizeof(Node*));
//    int rootCount = 0;

    // for each vertex
    for(i=0;i<size;i++)
    {
        int vertex = (int)orderingArray[i]->vertex;
        //printf("1.vertex: %d\n", vertex);
        //printf("1.beginR: %d\n", beginR);
        int newBeginX, newBeginP, newBeginR;



        int isLeft = (vertex < L); 
        int allSameSide = 1;

        if (orderingArray[i]->laterDegree > 0) {
            for(int j=0; j<orderingArray[i]->laterDegree; j++)
            {
                int laterVertex = orderingArray[i]->later[j];
                //printf("2.later: %d", laterVertex);
       
                if((isLeft && laterVertex >= L) || (!isLeft && laterVertex < L))
                {
                    allSameSide = 0;
                    break;
                }
            }
        }

        if(allSameSide) {
            //printf("3.vertex: %d\n", vertex);
            //beginR = beginR + 1; 
            //beginP = beginR;
            //beginX = beginR;
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
        // later neighbors, and avoiding earlier neighbors 
        int drop = 0;
        int rsize = 1;
        //printf("1.vertex:%d\n", vertex);


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
                                            newBeginX, newBeginP, newBeginR, max_k, nCalls, sumP, sqP, rsize, drop, root 
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


void listAllCliquesDegeneracy_A_maximal(double * cliqueCounts, NeighborListArray** orderingArray,
                                int size, int max_k, double *nCalls, double *sumP, double *sqP, Node*** rootNodes, int* rootCount)
{
    setConsoleToUTF8(); 
    *nCalls = (*nCalls) + 1;

    // vertex sets are stored in an array like this:
    // |--X--|--P--|
    int* vertexSets = (int *)Calloc(size, sizeof(int));

    // vertex i is stored in vertexSets[vertexLookup[i]]
    int* vertexLookup = (int *)Calloc(size, sizeof(int));

    int** neighborsInP = (int **)Calloc(size, sizeof(int*));
    int* numNeighbors = (int *)Calloc(size, sizeof(int));

    int i = 0;

    while(i<size)
    {
        vertexLookup[i] = i;
        vertexSets[i] = i;
        neighborsInP[i] = (int *)Calloc(1, sizeof(int));
        numNeighbors[i] = 1;
        i++;
    }

    int beginX = 0;
    int beginP = 0;
    int beginR = size;


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

        fillInPandXForRecursiveCallDegeneracyCliques( i, vertex,
                                                      vertexSets, vertexLookup,
                                                      orderingArray,
                                                      neighborsInP, numNeighbors,
                                                      &beginX, &beginP, &beginR,
                                                      &newBeginX, &newBeginP, &newBeginR);

        *sumP = *sumP + (newBeginR - newBeginP);
        *sqP = *sqP + ((newBeginR - newBeginP)*(newBeginR - newBeginP));


        printf("current vertex %d:\n", vertex);
        printf("R set elements：");
        for (int j = beginR; j < 4; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nX set elements：");
        for (int j = newBeginX; j < newBeginP; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nP set elements：");
        for (int j = newBeginP; j < newBeginR; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\n\n");

        // recursively compute maximal cliques containing vertex, some of its
        // later neighbors, and avoiding earlier neighbors 
        int drop = 0;
        int rsize = 1;
        printf("1.vertex:%d\n", vertex);

   
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
                                            newBeginX, newBeginP, newBeginR, max_k, nCalls, sumP, sqP, rsize, drop, root 
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
           all vertices in R, some vertices in P and no vertices in X.

    \param cliqueCount A pointer to the number of maximal cliques computed
                       thus far.

    \param cliques A linked list of cliques to return. <b>(only available when compiled
                   with RETURN_CLIQUES_ONE_BY_ONE defined)</b> 

    \param partialClique A linked list storing R, the partial clique for this
                         recursive call. 

    \param vertexSets An array containing sets of vertices divided into sets X, P, R and other.

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

    \param nCalls 

    \param rsize 

    \param drop 

*/
////
//void listAllCliquesDegeneracyRecursive_A( double * cliqueCounts,
//                                          int* vertexSets, int* vertexLookup,
//                                          int** neighborsInP, int* numNeighbors,
//                                          int beginX, int beginP, int beginR, int max_k,
//                                          double *nCalls, double *sumP, double *sqP,
//                                          int rsize, int drop)

void listAllCliquesDegeneracyRecursive_A(
        double * cliqueCounts,
        int* vertexSets, int* vertexLookup,
        int** neighborsInP, int* numNeighbors,
        int beginX, int beginP, int beginR, int max_k,
        double *nCalls, double *sumP, double *sqP,
        int rsize, int drop,
        Node* parent  
)
{

    *nCalls = (*nCalls) + 1;
    *sumP = *sumP + (beginR - beginP);
    *sqP = *sqP + ((beginR - beginP)*(beginR - beginP));


    //if ((beginP >= beginR) || (rsize-drop > max_k))
    ////
    if ((beginP >= beginR && beginX >= beginR) || (rsize-drop > max_k))
    {
        for (int i=drop; (i>=0) && (rsize-i <= max_k); i--)
        {
            int k = rsize - i;
            //cliqueCounts[k] += nCr[drop][i];
        }
        parent->is_leaf = true;////

      
        //printf("current vertex %d:\n", vertex);
        printf("R set element：");
        for (int j = beginR; j < 4; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\n");

        return;
    }

    int* myCandidatesToIterateThrough;
    int numCandidatesToIterateThrough = 0;

    // get the candidates to add to R to make a maximal clique 

    int pivot = findBestPivotNonNeighborsDegeneracyCliques( &myCandidatesToIterateThrough,
                                                            &numCandidatesToIterateThrough,
                                                            vertexSets, vertexLookup,
                                                            neighborsInP, numNeighbors,
                                                            beginX, beginP, beginR);

    printf("2.pivot:%d\n", pivot);

    // add candidate vertices to the partial clique one at a time and
    // search for maximal cliques 
    if(numCandidatesToIterateThrough != 0)
    {
        int iterator = 0;
        while(iterator < numCandidatesToIterateThrough)
        {
            // vertex to be added to the partial clique
            int vertex = myCandidatesToIterateThrough[iterator];


            Node* child = create_node(vertex, false);
            add_child(parent, child);
            printf("3.vertex:%d\n", vertex);

            int newBeginX, newBeginP, newBeginR;

            // add vertex into partialClique, representing R.

            // swap vertex into R and update all data structures 

            moveToRDegeneracyCliques( vertex,
                                      vertexSets, vertexLookup,
                                      neighborsInP, numNeighbors,
                                      &beginX, &beginP, &beginR,
                                      &newBeginX, &newBeginP, &newBeginR);



            // recursively compute maximal cliques with new sets R, P and X 
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


            if (vertex == pivot) {
                listAllCliquesDegeneracyRecursive_A(
                        cliqueCounts, vertexSets, vertexLookup,
                        neighborsInP, numNeighbors,
                        newBeginX, newBeginP, newBeginR, max_k,
                        nCalls, sumP, sqP,
                        rsize+1, drop+1,
                        child 
                );
            } else {
                listAllCliquesDegeneracyRecursive_A(
                        cliqueCounts, vertexSets, vertexLookup,
                        neighborsInP, numNeighbors,
                        newBeginX, newBeginP, newBeginR, max_k,
                        nCalls, sumP, sqP,
                        rsize+1, drop,
                        child  
                );
            }


            moveFromRToXDegeneracyCliques( vertex,
                                           vertexSets, vertexLookup,
                                           &beginX, &beginP, &beginR );

            iterator++;
        }


        printf("1.Recursive call (current depth %d) :\n", rsize);
        printf("R set element：");
        for (int j = beginR; j < 4; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nX set element（from %d-%d）:", beginX, beginP - 1);
        for (int j = beginX; j < beginP; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nP set element（from %d-%d）:", beginP, beginR - 1);
        for (int j = beginP; j < beginR; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\n\n");


        // swap vertices that were moved to X back into P, for higher recursive calls. 
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


        printf("2.Recursive call (current depth %d) :\n", rsize);
        printf("R set element：");
        for (int j = beginR; j < 4; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nX set element（from %d-%d）:", beginX, beginP - 1);
        for (int j = beginX; j < beginP; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\nP set element（from %d-%d）:", beginP, beginR - 1);
        for (int j = beginP; j < beginR; j++) {
            printf("%d ", vertexSets[j]);
        }
        printf("\n\n");

    }


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

    std::list<int> partialClique;


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
                                                 newBeginX, newBeginP, newBeginR, prefixTreeRoot);

#ifdef PRINT_CLIQUES_TOMITA_STYLE
        printf("b ");
#endif

        beginR = beginR + 1;

        partialClique.pop_back();
    }

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
                                                                   std::list<int> &partialClique,
                                                                   int* vertexSets, int* vertexLookup,
                                                                   int** neighborsInP, int* numNeighbors,
                                                                   int beginX, int beginP, int beginR, Node* root)
{

    // if X is empty and P is empty, process partial clique as maximal
    if(beginX >= beginP && beginP >= beginR)
    {
        partialClique.sort();

        Node *current = root;
        for (int v : partialClique) {
            Node *newNode = create_node(v, false);
            add_child(current, newNode);
            current = newNode;
        }
        current->is_leaf = true;


        ////insertIntoPrefixTree(root, partialClique);

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
                                                     newBeginX, newBeginP, newBeginR, root);

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