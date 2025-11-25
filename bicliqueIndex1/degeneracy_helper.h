//
// Created by 16374 on 2025/2/27.
//

#ifndef BICLIQUEINDEX1_DEGENERACY_HELPER_H
#define BICLIQUEINDEX1_DEGENERACY_HELPER_H

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>

#include"misc.h"
#include"LinkedList.h"
#include"MemoryManager.h"

/*!
*/


/*! \struct NeighborList

    \brief For a given ordering, this stores later neighbors and earlier neighbors
           for a given vertex in linked lists.
*/

struct NeighborList
{
    int vertex; //!< the vertex that owns this neighbor list
    LinkedList* earlier; //!< a linked list of neighbors that come before this vertex in the ordering
    LinkedList* later; //!< a linked list of neighbors that come after this vertex in the ordering
    int orderNumber; //!< the position of this vertex in the ordering
};

typedef struct NeighborList NeighborList;

/*! \struct NeighborListArray

    \brief For a given ordering, this stores later neighbors and earlier neighbors
           for a given vertex in arrays.

    This version of the NeighborList structure is more cache efficient.
*/

struct NeighborListArray
{
    int vertex; //!< the vertex that owns this neighbor list
    int* earlier; //!< an array of neighbors that come before this vertex in an ordering
    int earlierDegree; //!< the number of neighbors in earlier
    int* later; //!< an array of neighbors that come after this vertex in an ordering
    int laterDegree; //!< an array of neighbors that come after this vertex in an ordering
    int orderNumber; //!< the position of this vertex in the ordering
};

typedef struct NeighborListArray NeighborListArray;

int computeDegeneracy(LinkedList** list, int size);

NeighborList** computeDegeneracyOrderList(LinkedList** list, int size);

NeighborListArray** computeDegeneracyOrderArray(LinkedList** list, int size);

int neighborListComparator(int* nl1, int* nl2);

#endif //BICLIQUEINDEX1_DEGENERACY_HELPER_H
