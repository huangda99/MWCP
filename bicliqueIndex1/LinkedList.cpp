//
// Created by 16374 on 2025/2/27.
//
#include"LinkedList.h"
#include"misc.h"
#include"MemoryManager.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_set>


/** \name Functions on Link structures
    \brief These functions operate on Link structures.
 */

//@{

/*! \brief tells if a given Link is the head sentinel.

    \param list A link structure.

    \return true if the link is the head sentinel, false otherwise.
*/

int isHead(Link* list)
{
#ifdef DEBUG
    printf("isHead...\n");
#endif

    assert(list != NULL);

    return (list->prev == NULL);
}

/*! \brief tells if a given Link is the tail sentinel.

    \param list A link.

    \return true if the link is the tail sentinel, false otherwise.
*/

int isTail(Link* list)
{
#ifdef DEBUG
    printf("isTail...\n");
#endif

    assert(list != NULL);

    return (list->next == NULL);
}

/*! \brief location-aware function to add a link after a given link.

    \param list The link that we want to add our data after.

    \param data A peice of data to put in the added link.

    \return A pointer to the Link that was added after list.
*/

Link* addAfter(Link* list, int data)
{
#ifdef DEBUG
    printf("addAfter...\n");
#endif

    assert(list != NULL);
    assert(list->next != NULL);

    Link* newLink = (Link*) Malloc(sizeof(Link));

    newLink->data = data;

    newLink->next = list->next;
    newLink->prev = list;

    list->next->prev = newLink;
    list->next = newLink;

#ifdef SMARTLENGTH
    newLink->linkedList = list->linkedList;
    newLink->linkedList->length++;
#endif

    return newLink;
}

/*! \brief location-aware function to add a link before a given link.

    \param list The link that we want to add our data before.

    \param data A peice of data to put in the added link.

    \return A pointer to the Link that was added after list.
*/

Link* addBefore(Link* list, int data)
{
#ifdef DEBUG
    printf("addBefore...\n");
#endif

    assert(list != NULL);
    assert(list->prev != NULL);

    Link* newLink = (Link*)Malloc(sizeof(Link));

    newLink->data = data;

    newLink->next = list;
    newLink->prev = list->prev;

    list->prev->next = newLink;
    list->prev = newLink;

#ifdef SMARTLENGTH
    newLink->linkedList = list->linkedList;
    newLink->linkedList->length++;
#endif

    return newLink;
}

/*! \brief delete the given link, and return its data.

    \param list The link that we want to get rid of.

    \return the data that was in the link, if it was
            allocated by you, you need to free it..
*/

int deleteLink(Link* list)
{
#ifdef DEBUG
    printf("delete...\n");
#endif
    assert(list != NULL);
    assert(list->next != NULL);
    assert(list->prev != NULL);

    int data = list->data;

    Link* linkToFree = removeLink(list);

    Free(linkToFree);

    return data;
}

/*! \brief location-aware method to add a link before another link.

    \param list The link that we want to add a link before.

    \param newLink The Link to be added after list.
*/

void addLinkBefore(Link* list, Link* newLink)
{
    assert(list != NULL);
    assert(list->prev != NULL);
    assert(newLink != NULL);

    newLink->next = list;
    newLink->prev = list->prev;

    newLink->next->prev = newLink;
    newLink->prev->next = list->prev;

#ifdef SMARTLENGTH
    newLink->linkedList = list->linkedList;
    newLink->linkedList->length++;
#endif

}

/*! \brief location-aware method to remove a link, and return it.

    \param list The link that we want remove and return.

    \return the removed link
*/

Link* removeLink(Link* list)
{
#ifdef DEBUG
    printf("removeLink...\n");
#endif
    assert(list != NULL);
    assert(list->next != NULL);
    assert(list->prev != NULL);

    list->next->prev = list->prev;
    list->prev->next = list->next;

    list->next = NULL;
    list->prev = NULL;

#ifdef SMARTLENGTH
    list->linkedList->length--;
    list->linkedList = NULL;
#endif

    return list;
}

//@}

/** \name Functions on LinkedList structures
    \brief These functions operate on LinkedList structures.
 */

//@{

/*! \brief create a new empty linked list

    \return the created linked list
*/

LinkedList* createLinkedList(void)
{
    LinkedList* linkedList = (LinkedList*) Malloc(sizeof(LinkedList));

    linkedList->head = (Link *) Malloc(sizeof(Link));
    linkedList->tail = (Link *) Malloc(sizeof(Link));

    linkedList->head->prev = NULL;
    linkedList->head->next = linkedList->tail;
    linkedList->head->data = (int) 0xDEAD0000;

    linkedList->tail->prev = linkedList->head;
    linkedList->tail->next = NULL;
    linkedList->tail->data = (int) 0xDEADFFFF;
#ifdef SMARTLENGTH
    linkedList->length = 0;
    linkedList->head->linkedList = linkedList;
    linkedList->tail->linkedList = linkedList;
#endif

    return linkedList;
}

/*! \brief destroy a linked list

    If you allocated data that is in each link, then
    this will cause a memory leak for you.

    \see destroyLinkedListWithClean

    \param linkedList The linked list to destroy.
*/

void destroyLinkedList(LinkedList* linkedList)
{
    Link* curr = linkedList->head;

    while(curr != NULL)
    {
        Link* currNext = curr->next;
        Free(curr);
        curr = currNext;
    }

    Free(linkedList);
}

/*! \brief destroy a linked list and run a clean function
           on the data in each link.

    \param linkedList The linked list to destroy.

    \param clean A pointer to a function that cleans the data in the links.
*/

void destroyLinkedListWithClean(LinkedList* linkedList, void (*clean)(int))
{
    Link* curr = linkedList->head;

    while(curr != NULL)
    {
        Link* currNext = curr->next;
        clean(curr->data);
        Free(curr);
        curr = currNext;
    }

    Free(linkedList);
}

/*! \brief copy a linked list

    \param destination copy the linked list here

    \param source copy this linked list
*/

void copyLinkedList(LinkedList* destination,
                    LinkedList* source)
{
    assert(destination != NULL && source != NULL);

    Link* curr = source->head->next;

    while(!isTail(curr))
    {
        addLast(destination, curr->data);
        curr = curr->next;
    }
}

/*! \brief Compare two linked lists to see if they are equal.

    \param list1 A linked list.

    \param list2 Another linked list.

    \param comparator A function to compare data in the links copy this linked list.

    \return true if the input linked lists have the same data in the same order.
*/

int equal( LinkedList* list1,
           LinkedList* list2,
           int (*comparator)(int,int))
{
    assert(list1 != NULL && list2 !=NULL);

    Link* curr1 = list1->head->next;
    Link* curr2 = list2->head->next;

    while(!isTail(curr1) && !isTail(curr2))
    {
        if(comparator(curr1->data, curr2->data) == 0)
        {
            curr1 = curr1->next;
            curr2 = curr2->next;
        }
        else if(comparator(curr1->data, curr2->data) > 0)
        {
            return 0;
            curr2 = curr2->next;
        }
        else
        {
            return 0;
            curr1 = curr1->next;
        }
    }

    return (isTail(curr1) && isTail(curr2));
}

/*! \brief decide if a linked list contains a piece of data.

    \param linkedList A linked list.

    \param data The data that we want to look for in linkedList.

    \param comparator A function that returns 0 when two data elements are equal.

    \return true if linkedList contains data.
*/

int contains(LinkedList* linkedList, int data, int (*comparator)(int,int))
{
    assert(linkedList != NULL);

    Link* curr = linkedList->head->next;

    while(!isTail(curr))
    {
        if(comparator(curr->data, data) == 0)
        {
            return 1;
        }
        curr = curr->next;
    }

    return 0;
}

/*! \brief A location-aware function to add data to the beginning of a linked list.

    \param linkedList A linked list.

    \param data The data that we want to add to the beginning of linkedList.

    \return The link where data was placed in the linked list.
*/

Link* addFirst(LinkedList* linkedList, int data)
{
    assert(linkedList != NULL);

    return addAfter(linkedList->head, data);
}

/*! \brief A location-aware function to add data to the end of a linked list.

    \param linkedList A linked list.

    \param data The data that we want to add to the end of linkedList.

    \return The link where data was placed in the linked list.
*/

Link* addLast(LinkedList* linkedList, int data)
{
    assert(linkedList != NULL);

    return addBefore(linkedList->tail, data);
}

/*! \brief return the first piece of data in a linked list

    \param linkedList A linked list.

    \return The data in the first link of the linked list.
*/

int getFirst(LinkedList* linkedList)
{
#ifdef DEBUG
    printf("getFirst...\n");
#endif
    assert(linkedList != NULL);
    assert(!isEmpty(linkedList));

    return linkedList->head->next->data;
}

/*! \brief remove the first link from a linked list

    \param linkedList A linked list.

    \return The first link of the linked list.
*/

Link* removeFirst(LinkedList* linkedList)
{
    assert(linkedList != NULL);

    if(!isEmpty(linkedList))
        return removeLink(linkedList->head->next);

    return NULL;
}

/*! \brief Remove and return the last link from a linked list.

    \param linkedList A linked list.

    \return The last link of the linked list.
*/

Link* removeLast(LinkedList* linkedList)
{
    assert(linkedList != NULL);

    if(!isEmpty(linkedList))
        return removeLink(linkedList->tail->prev);

    return NULL;
}

/*! \brief delete the last link in the linked list

    \param linkedList A linked list.
*/

void deleteLast(LinkedList* linkedList)
{
    assert(linkedList != NULL);
    if(!isEmpty(linkedList))
        deleteLink(linkedList->tail->prev);

    return;
}

/*! \brief Print the first 10 items in the linked list

    \param linkedList A linked list.

    \param printFunc A function to print the data elements in
                     the linked list.
*/

void printListAbbv(LinkedList* linkedList, void (*printFunc)(int))
{
#ifdef DEBUG
    printf("printListAbbv...\n");
#endif
    Link* curr = linkedList->head;
    curr = curr->next;

    int count = 0;

    while(!isTail(curr) && count != 10)
    {
        count++;
        printFunc(curr->data);
        if(!isTail(curr->next))
        {
            printf(",");
        }
        curr = curr->next;
    }

    if(!isTail(curr))
    {
        printf("... plus %d more", length(linkedList)-10);
    }

    printf("\n");

}

/*! \brief Print the items in the linked list.

    \param linkedList A linked list.

    \param printFunc A function to print the data elements in
                     the linked list.
*/

void printList(LinkedList* linkedList, void (*printFunc)(int))
{
#ifdef DEBUG
    printf("printList...\n");
#endif
    Link* curr = linkedList->head->next;

    while(!isTail(curr))
    {
        printFunc(curr->data);
        if(!isTail(curr->next))
        {
            printf(" ");
        }
        curr = curr->next;
    }

    printf("\n");

}

/*! \brief Compute the number of data elements in a linked list.

    \param linkedList A linked list.

    \return The number of data elements in the linked list.
*/

int length(LinkedList* linkedList)
{
#ifdef DEBUG
    printf("length...\n");
#endif

#ifndef SMARTLENGTH
    int length = 0;
    Link* curr = linkedList->head->next;

    while(!isTail(curr))
    {
        length++;
        curr = curr->next;
    }

    return length;
#else
    assert(linkedList != NULL);
    return linkedList->length;
#endif
}

/*! \brief Determine if a linked list is empty.

    \param linkedList A linked list.

    \return Non-zero if the linked list is empty, zero otherwise.
*/

int isEmpty(LinkedList* linkedList)
{
#ifdef DEBUG
    printf("isEmpty...\n");
#endif

    assert(linkedList != NULL);

    return isTail(linkedList->head->next);
}

/* 读取二分图并构建邻接表 */
LinkedList** readBipartiteGraph(int* L, int* R, int* n, int* m, char* fpath, std::vector<float>& vertexWeight) {
    FILE* fp = fopen(fpath, "r");
    if (!fp) {
        fprintf(stderr, "Could not open input file.\n");
        exit(1);
    }

    // 读取左右侧顶点数量
    if (fscanf(fp, "%d %d", L, R) != 2) {
        fprintf(stderr, "Invalid file format: missing L and R\n");
        exit(1);
    }

    *n = *L + *R;   // 总顶点数 = 左 + 右
    *m = 0;          // 初始化边数

    // 创建邻接表（包含左右侧所有顶点）
    LinkedList** adjList = (LinkedList**)Calloc(*n, sizeof(LinkedList*));
    for (int i = 0; i < *n; i++) {
        adjList[i] = createLinkedList();
    }

    //存储顶点的权重
    vertexWeight.resize(*n);
    // 读取边并转换编号
    int u, v;
    float weight;
    while (fscanf(fp, "%d %d %f", &u, &v, &weight) == 3) {
        // 输入编号假设为1-based，转换为0-based
        int left = u - 1;         // 左侧顶点: 0 ~ L-1
        int right = *L + (v - 1); // 右侧顶点: L ~ L+R-1
        vertexWeight[left] += weight;
        vertexWeight[right] += weight;//将边权重转换为顶点权重

        // 添加双向边（无向图）
        addLast(adjList[left], right);
        addLast(adjList[right], left);
        *m += 2; // 每条边存储两次
    }

    fclose(fp);
    return adjList;
}

// 辅助函数：用于整数排序
int compareInts(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
    return (arg1 > arg2) - (arg1 < arg2);
}


/* 将二分图转换为普通图，添加二跳边 */
/*
void convertBipartiteToNormalGraph(LinkedList** adjList, int L, int n) {
    for (int u = 0; u < n; u++) {
        // 确定顶点属于左侧（0~L-1）还是右侧（L~n-1）
        int isLeft = (u < L) ? 1 : 0;

        // 临时数组存储二跳顶点（最多n个）
        int* temp = (int*)malloc(n * sizeof(int));
        int count = 0;

        // 遍历u的所有一跳邻居w
        Link* neighbor = adjList[u]->head->next;
        while (neighbor != adjList[u]->tail) {//
            int w = neighbor->data;

            // 遍历w的所有邻居v
            Link* secondNeighbor = adjList[w]->head->next;
            while (secondNeighbor != adjList[w]->tail) {
                int v = secondNeighbor->data;

                // 检查v是否满足以下条件：
                // 1. v与u同侧（左侧或右侧）
                // 2. v > u（避免重复添加）
                if ((isLeft && v < L) || (!isLeft && v >= L)) {
                    if (v > u) {
                        temp[count++] = v;
                    }
                }
                secondNeighbor = secondNeighbor->next;
            }
            neighbor = neighbor->next;
        }

        // 去重步骤：排序 -> 去重
        if (count > 0) {
            qsort(temp, count, sizeof(int), compareInts);
            int uniqueCount = 0;
            for (int i = 0; i < count; i++) {
                if (i == 0 || temp[i] != temp[i-1]) {
                    temp[uniqueCount++] = temp[i];
                }
            }

            // 添加唯一边（双向）
            for (int i = 0; i < uniqueCount; i++) {
                int v = temp[i];
                addLast(adjList[u], v);
                addLast(adjList[v], u);
            }
        }
        //printf("=====================");

        free(temp);
    }
}
*/

/* 辅助函数：深拷贝一个链表 */
LinkedList* copyLinkedList(LinkedList* list) {
    LinkedList* newList = createLinkedList();
    Link* curr = list->head->next;
    while (curr != list->tail) {
        addLast(newList, curr->data);
        curr = curr->next;
    }
    return newList;
}

/*
   将二分图转换为普通图，添加二跳边。
   第二种方法：先复制原始图，再基于副本遍历，防止遍历过程中新添加的边干扰结果。
*/
/*
void convertBipartiteToNormalGraph(LinkedList** adjList, int L, int n) {
    // 复制原始图的邻接表
    LinkedList** originalAdjList = (LinkedList**) Calloc(n, sizeof(LinkedList*));
    for (int i = 0; i < n; i++) {
        originalAdjList[i] = copyLinkedList(adjList[i]);
    }
    printf("1.1=====================\n");

    // 遍历每个顶点 u，基于原始副本计算二跳邻居
    for (int u = 0; u < n; u++) {
        int isLeft = (u < L) ? 1 : 0;
        // 用于存储 u 的二跳邻居（最多 n 个）
        int* temp = (int*) malloc(n * sizeof(int));
        int count = 0;

        // 遍历 u 的一跳邻居（从原始副本中读取）
        Link* neighbor = originalAdjList[u]->head->next;
        while (neighbor != originalAdjList[u]->tail) {
            int w = neighbor->data;
            // 遍历 w 的一跳邻居，即 u 的二跳邻居（同样从副本中读取）
            Link* secondNeighbor = originalAdjList[w]->head->next;
            while (secondNeighbor != originalAdjList[w]->tail) {
                int v = secondNeighbor->data;
                // 条件：u 和 v 同侧，并且 v > u（避免重复添加）
                if ((isLeft && v < L) || (!isLeft && v >= L)) {
                    if (v > u) {
                        temp[count++] = v;
                    }
                }
                secondNeighbor = secondNeighbor->next;

            }
            neighbor = neighbor->next;
        }

        // 排序并去重
        if (count > 0) {
            qsort(temp, count, sizeof(int), compareInts);
            int uniqueCount = 0;
            for (int i = 0; i < count; i++) {
                if (i == 0 || temp[i] != temp[i-1]) {
                    temp[uniqueCount++] = temp[i];
                }
            }
            // 添加唯一边（双向添加）
            for (int i = 0; i < uniqueCount; i++) {
                int v = temp[i];
                addLast(adjList[u], v);
                addLast(adjList[v], u);
            }
        }
        free(temp);
    }
    printf("1.2=====================\n");

    // 释放原始图的副本
    for (int i = 0; i < n; i++) {
        destroyLinkedList(originalAdjList[i]);
    }
    Free(originalAdjList);
}
 */
////改  有重复的
void convertBipartiteToNormalGraph(LinkedList** adjList, int L, int n) {
    // 复制原始图的邻接表
    LinkedList** originalAdjList = (LinkedList**) Calloc(n, sizeof(LinkedList*));
    for (int i = 0; i < n; i++) {
        originalAdjList[i] = copyLinkedList(adjList[i]);
    }
    //printf("1.1=====================\n");

    // 遍历每个顶点 u，基于原始副本计算二跳邻居
    for (int u = 0; u < n; u++) {
        int isLeft = (u < L) ? 1 : 0;
        // 用于存储 u 的二跳邻居（最多 n 个）
        int* temp = (int*) malloc(n * sizeof(int));
        std::unordered_set<int> twoHopNei;
        //int count = 0;

        // 遍历 u 的一跳邻居（从原始副本中读取）
        Link* neighbor = originalAdjList[u]->head->next;
        while (neighbor != originalAdjList[u]->tail) {
            int w = neighbor->data;
            // 遍历 w 的一跳邻居，即 u 的二跳邻居（同样从副本中读取）
            Link* secondNeighbor = originalAdjList[w]->head->next;
            while (secondNeighbor != originalAdjList[w]->tail) {
                int v = secondNeighbor->data;
                //printf("u: %d, v: %d\n", u, v);
                if (twoHopNei.find(v) != twoHopNei.end()) {
                    secondNeighbor = secondNeighbor->next;
                    continue;
                }
                twoHopNei.insert(v);
                // 条件：u 和 v 同侧，并且 v > u（避免重复添加）
                if ((isLeft && v < L) || (!isLeft && v >= L)) {
                    if (v > u) {

                        //printf("u: %d, v: %d\n", u, v);
                        addLast(adjList[u], v);
                        addLast(adjList[v], u);
                    }
                }
                secondNeighbor = secondNeighbor->next;

            }
            neighbor = neighbor->next;
        }

        /*
        // 排序并去重
        if (count > 0) {
            qsort(temp, count, sizeof(int), compareInts);
            int uniqueCount = 0;
            for (int i = 0; i < count; i++) {
                if (i == 0 || temp[i] != temp[i-1]) {
                    temp[uniqueCount++] = temp[i];
                }
            }
            // 添加唯一边（双向添加）
            for (int i = 0; i < uniqueCount; i++) {
                int v = temp[i];
                addLast(adjList[u], v);
                addLast(adjList[v], u);
            }
        }
        */
        free(temp);
    }
    //printf("1.2=====================\n");

    // 释放原始图的副本
    for (int i = 0; i < n; i++) {
        destroyLinkedList(originalAdjList[i]);
    }
    Free(originalAdjList);
}

