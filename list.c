#include "list.h"
#include <stdio.h>

/*

IDEA: upon setup, I link the pool of 10 heads together and the pool of 100 nodes together.
If I ever need to add/remove, I have a global head pointer for each linked list so I can either
remove the head (and update the new head pointer to be the next node) or add it back to the list
and set the newly added node as the head (linking accordingly). This ensures O(1).

This is similar to the stack data structure with methods pop/push, where I either pop off the 
head/current node, or push it back onto the linked list where it now becomes the new head.

*/

static bool first_run = true;

static List lists[LIST_MAX_NUM_HEADS];
static Node nodes[LIST_MAX_NUM_NODES];

// keeps tracks of the current node/list position in the linked list
static List* listHead;
static Node* nodeHead;

static void setup() {
    
    // non-constant time setup
    for (int i = 0; i < LIST_MAX_NUM_HEADS; i++) {

        List list = {
            NULL,
            NULL, 
            NULL,
            NULL,
            0,
            false,
            false
        };

        lists[i] = list;

    }

    for (int i = 0; i < LIST_MAX_NUM_NODES; i++) {
        
        Node node = {
            NULL,
            NULL,
            NULL
        };

        nodes[i] = node;

    }

    // linking all the heads together, and nodes together
    for (int i = 0; i < LIST_MAX_NUM_HEADS - 1; i++) {
        lists[i].next = &lists[i + 1];
    }

    for (int i = 1; i < LIST_MAX_NUM_NODES - 1; i++) {
        nodes[i].next = &nodes[i + 1];
        nodes[i].prev = &nodes[i - 1];

    }

    // setup the first and last nodes, since they are special cases
    // with regards to the next + prev pointers
    nodes[0].next = &nodes[1];
    nodes[LIST_MAX_NUM_NODES - 1].prev = &nodes[LIST_MAX_NUM_NODES - 2];

    listHead = &lists[0];
    nodeHead = &nodes[0];
}

// extracts the head list from the linked list of heads
// updates the new current position too
static List* createNewList(List* list) {
    
    List* secondPos = listHead->next;
    listHead->next = NULL;

    List* newList = listHead;
    listHead = secondPos;

    return newList;
}

// links the node into place, then updates the nodeHead
static void linkNewNode(List* pList, Node* before, Node* after, void* pItem) {

    Node* secondPos = nodeHead->next;
    Node* newNode = nodeHead;
    newNode->item = pItem;
    nodeHead = secondPos;
    
    // checks if we're inserting at the start, mid, or end respectively
    if (before == NULL && after == NULL) {
        newNode->next = NULL;
        newNode->prev = NULL;

        pList->head = newNode;
        pList->tail = newNode;
        pList->size = 1;
        pList->current = newNode;

    } else if (before != NULL && after != NULL) {
        newNode->prev = before;
        newNode->next = after;
        before->next = newNode;
        after->prev = newNode;

        pList->current = newNode;
        pList->size += 1;

    } else if (before != NULL && after == NULL) {
        before->next = newNode;
        newNode->prev = before;

        newNode->next = NULL;

        pList->current = newNode;
        pList->size += 1;
        pList->tail = newNode;

    } else {
        newNode->prev = NULL;
        newNode->next = after;
        after->prev = newNode;

        pList->current = newNode;
        pList->size += 1;
        pList->head = newNode;

    }
}

// helper function for remove, trim
static void* extractNodeFromList(List* pList, Node* before, Node* after, Node* extractedNode, bool setCurrNext) {
    void* extractedItem = extractedNode->item;

    if (before != NULL && after != NULL) {
        before->next = after;
        after->prev = before;

        pList->current = after;

    } else if (before == NULL && after != NULL) {
        after->prev = NULL;
        pList->current = after;
        pList->head = after;

    } else if (before != NULL && after == NULL) {
        before->next = NULL;

        if (setCurrNext) {
        
            pList->current = NULL;
            pList->tailOutOfBounds = true;
        } else {
            pList->current = before;
        }
        pList->tail = before;

    } else {
        pList->head = NULL;
        pList->tail = NULL;
        pList->current = NULL;
        pList->tailOutOfBounds = false;
        pList->headOutOfBounds = false;
        pList->next = NULL;
    }

    pList->size -= 1;
    extractedNode->item = NULL;
    extractedNode->prev = NULL;
    extractedNode->next = nodeHead;
    nodeHead = extractedNode;

    return extractedItem;
}

static bool empty(List* pList) {
    return List_count(pList) == 0;
}

List* List_create() {
    
    List* returnList = NULL;

    if (first_run) {
        setup();
        first_run = false;
    } 

    if (listHead != NULL) {
        returnList = createNewList(NULL);
    }

    return returnList;

}

int List_count(List* pList) {
    return pList->size;
}

void* List_first(List* pList) {
    if (empty(pList)) {
        pList->current = NULL;
        return NULL;
    }

    pList->headOutOfBounds = false;
    pList->tailOutOfBounds = false;
    pList->current = pList->head;
    return pList->current->item;
}

void* List_last(List* pList) {
    if (empty(pList)) {
        pList->current = NULL;
        return NULL;
    }

    pList->headOutOfBounds = false;
    pList->tailOutOfBounds = false;
    pList->current = pList->tail;
    return pList->current->item;

}

void* List_next(List* pList) {

    if (empty(pList)) {
        return NULL;
    }

    if (pList->tailOutOfBounds) {
        return NULL;
    } 

    if (pList->headOutOfBounds) {
        pList->current = pList->head;
        pList->headOutOfBounds = false;

    } else {
        pList->current = pList->current->next;
        pList->tailOutOfBounds = false;
    }

    if (pList->current == NULL) {
        pList->tailOutOfBounds = true;
        return NULL;
    }

    return pList->current->item;
}

void* List_prev(List* pList) {

    if (empty(pList)) {
        return NULL;
    }

    if (pList->headOutOfBounds) {
        return NULL;
    }

    if (pList->tailOutOfBounds) {
        pList->current = pList->tail;
        pList->tailOutOfBounds = false;
    } else {
        pList->current = pList->current->prev;
        pList->headOutOfBounds = false;
    }

    if (pList->current == NULL) {
        pList->headOutOfBounds = true;
        return NULL;
    }

    return pList->current->item;
}

void* List_curr(List* pList) {
    if (empty(pList) || pList->headOutOfBounds || pList->tailOutOfBounds) {
        return NULL;
    }

    return pList->current->item;
}

int List_add(List* pList, void* pItem) {

    if (nodeHead != NULL) {

        if (empty(pList)) {

            linkNewNode(pList, NULL, NULL, pItem);

        } else if (pList->current == pList->tail) {

            linkNewNode(pList, pList->current, NULL, pItem);

        } else if (pList->headOutOfBounds) {

            pList->headOutOfBounds = false;

            linkNewNode(pList, NULL, pList->head, pItem);

        } else if (pList->tailOutOfBounds) {

            pList->tailOutOfBounds = false;
            linkNewNode(pList, pList->tail, NULL, pItem);

        } else {
            linkNewNode(pList, pList->current, pList->current->next, pItem);

        }

        return 0;
    }

    return -1;
}

int List_insert(List* pList, void* pItem) {

    if (nodeHead != NULL) {
        
        if (empty(pList)) {
            linkNewNode(pList, NULL, NULL, pItem);

        } else if (pList->current == pList->head) {

            linkNewNode(pList, NULL, pList->current, pItem);

        } else if (pList->headOutOfBounds) {

            pList->headOutOfBounds = false;
            linkNewNode(pList, NULL, pList->head, pItem);

        } else if (pList->tailOutOfBounds) {

            pList->tailOutOfBounds = false;
            linkNewNode(pList, pList->tail, NULL, pItem);

        } else {

            linkNewNode(pList, pList->current->prev, pList->current, pItem);

        }

        return 0;
    }

    return -1;
}

int List_append(List* pList, void* pItem) {

    if (nodeHead != NULL) {

        if (empty(pList)) {

            linkNewNode(pList, NULL, NULL, pItem);

        } else {

            linkNewNode(pList, pList->tail, NULL, pItem);

        }

        return 0;
    }

    return -1;
}

int List_prepend(List* pList, void* pItem) {

    if (nodeHead != NULL) {

        if (empty(pList)) {

            linkNewNode(pList, NULL, NULL, pItem);

        } else {

            linkNewNode(pList, NULL, pList->head, pItem);

        }
        return 0;
    }

    return -1;
}

void* List_remove(List* pList) {
    if (empty(pList) || pList->headOutOfBounds || pList->tailOutOfBounds) {
        return NULL;
    }

    void* pItem = extractNodeFromList(pList, pList->current->prev, pList->current->next, pList->current, true);

    if (empty(pList)) {

        pList->current = NULL;
        pList->head = NULL;
        pList->tail = NULL;
        pList->next = listHead;
        listHead = pList;

    } 

    return pItem;
}

void* List_trim(List* pList) {
    if (empty(pList)) {
        return NULL;

    } 

    void* pItem = extractNodeFromList(pList, pList->tail->prev, pList->tail->next, pList->tail, false);
    return pItem;

}

void List_concat(List* pList1, List* pList2) {

    if (!empty(pList1) && !empty(pList2)) {

        pList1->tail->next = (pList2->head);
        pList2->head->prev = (pList1->tail);

        int p1 = pList1->size;
        int p2 = pList2->size;

        pList1->size = p1 + p2;
        pList1->tail = pList2->tail;

        pList2->current = NULL;
        pList2->head = NULL;
        pList2->tail = NULL;
        pList2->size = 0;
        pList2->headOutOfBounds = false;
        pList2->tailOutOfBounds = false;
        pList2->next = listHead;
        listHead = pList2;

    } else if (empty(pList1) && !empty(pList2)) {

        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
        pList1->current = pList2->head;
        pList1->headOutOfBounds = false;
        pList1->tailOutOfBounds = false;
        pList1->size = pList2->size;

        pList2->next = listHead;
        pList2->current = NULL;
        pList2->size = 0;
        pList2->headOutOfBounds = false;
        pList2->tailOutOfBounds = false;
        pList2->tail = NULL;
        pList2->head = NULL;

        listHead = pList2;

    } else if (!empty(pList1) && empty(pList2)) {

        pList2->next = listHead;
        listHead = pList2;

    }

}

void List_free(List* pList, FREE_FN pItemFreeFn) {

    if (empty(pList)) {

        pList->next = listHead;
        listHead = pList;

    } else {
        while (!empty(pList)) {

            void* pItem = extractNodeFromList(pList, NULL, pList->head->next, pList->head, true);
            (*pItemFreeFn)(pItem);
            
        }

        pList->next = listHead;
        listHead = pList;
    }
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {

    if (empty(pList) || pList->tailOutOfBounds) {
        return NULL;
    }

    if (pList->headOutOfBounds) {
        pList->headOutOfBounds = false;
        pList->current = pList->head;
    }

    while (pList->current != NULL) {
        
        if ((*pComparator)((pList->current->item), (pComparisonArg)) == 1) {
            return pList->current->item;
        }

        pList->current = pList->current->next;
    }

    pList->tailOutOfBounds = true;

    return NULL;
}

void* List_pop(List* pList) {
    if (!empty(pList)) {
        
        List_first(pList);
        return List_remove(pList);
    }

    return NULL;
}