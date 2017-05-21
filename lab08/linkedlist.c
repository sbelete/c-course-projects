#include <stdlib.h>
#include <stdio.h>

#include "./linkedlist.h"

/* A ListNode is a node in a singly-linked list */
typedef struct ListNode ListNode;
struct ListNode {
    void *value;
    ListNode *next;
};

struct LinkedList {
    // Function to compare two elements for equality
    int (*equals)(void *, void *);
    // Function that copies an element and returns a pointer to the copy.
    void *(*copy)(void *);
    // Function to delete an element
    void (*delete)(void *);

    ListNode *head;  // The first element (remember to update)
    int size;  // The number of elements in the list (remember to update)
};

/******************************
 * Creation/deletion functions
 ******************************/

/* Create a new linkedlist with the given element functions and return it */
LinkedList *linkedlist_new(int (*equals)(void *, void *),
                    void *(*copy)(void *), 
                    void (*delete)(void *)) {

    LinkedList *llist = (LinkedList *)malloc(sizeof(LinkedList));
    llist->equals = equals;
    llist->copy = copy;
    llist->delete  = delete;
    llist->size = 0;

    llist->head = (ListNode *)malloc(sizeof(ListNode));
    llist->head = NULL;
    // TODO: 1. Allocate space for a linkedlist struct.
    //       2. Initialize the struct.
    return llist;
}

/* Delete the given linked list */
void linkedlist_delete(LinkedList *linkedlist) {
    // TODO: 1. Delete all of the linked list's internal data.
    //       2. Free the struct.
    LinkedList *llst = linkedlist;
    ListNode *l1 = llst->head;
    ListNode *l2 = l1->next;

    while(l1 != NULL){
        free(l1->value);
        free(l1);
        l1 = l2;
        l2 = l1->next;
    }

    //free(l2);
    free(llst);
    free(linkedlist);
}


/******************************
 * Access/modification functions
 ******************************/

/* Add a copy of the given element to the tail of the list */
void linkedlist_append(LinkedList *linkedlist, void *element) {
    // TODO: 1. Find the last node in the linked list.
    //       2. Create a copy of the element and store the copy in
    //          a new list node.
    //       3. Set the next pointer of the last node to the newly
    //          created node.
    ListNode *l1 = NULL;

    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
    temp->value = linkedlist->copy(element);
    temp->next = NULL;

    if(linkedlist->size == 0){
        linkedlist->head = temp;
        linkedlist->size += 1;
        return;
    }

    l1 = linkedlist->head;
    
    while(l1->next != NULL) {l1 = l1->next;}

    l1->next = temp;
    l1 = l1->next;
    
    linkedlist->size += 1;
}


/* Insert a copy of the given element at the given index (before the element 
 * that currently has that index).
 * Inserting at size is equivalent to appending.
 * Return 1 if the element was added successfully
 * 0 otherwise (if the index is invalid)
 */
int linkedlist_insert(LinkedList *linkedlist, void *element, int index) {
    // TODO: 1. Find the node at the given index, if such a node exists.
    //       2. Create a copy of the element and store the copy in
    //          a new list node.
    //       3. Update the next pointers of the old and new nodes.
    ListNode *l1 = linkedlist->head;
    int i =0;
    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
    temp->value = linkedlist->copy(element);
    //temp->next = l1;

    while(l1 != NULL && i != index){
        if(i == index -1){
            temp->next = linkedlist->copy(l1->next);
            l1->next = temp;
            linkedlist->size += 1;
            return 1;
        }
        l1 = l1->next;
        i++;
    }

    if(i != index){return 0;}

    temp->value = linkedlist->copy(element);
    temp->next = l1;

    if(linkedlist->size == 0){
        linkedlist->head = temp;
        linkedlist->size += 1;
        return 1;
    }
    if(i == 0){
        linkedlist->head = temp;
        linkedlist->size += 1;
        return 1;
    }

    linkedlist->size += 1;

    return 1;
}

/* Return 1 if the given element is in the list
 * 0 otherwise
 */
int linkedlist_contains(LinkedList *linkedlist, void *element) {
    ListNode *current = linkedlist->head;
    while (current != NULL) {
        if (linkedlist->equals(current->value, element)) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/* Remove the first occurence of the given element from the list
 * Return 1 if the element was removed successfully
 * 0 otherwise (if the element was not found)
 */
int linkedlist_remove(LinkedList *linkedlist, void *element) {
    // TODO: 1. Find the first node containing the element.
    //       2. If an element is found, delete the linkedlist's copy
    //          of the element and remove the node.
    //       3. Update the next pointer of the previous element.
    if(!linkedlist_contains(linkedlist, element)) {return 0;}

    ListNode *l1 = linkedlist->head;
    
    while(l1 != NULL){
        if(linkedlist->equals(l1->value, element)){
            break;
        }
        l1 = l1->next;
    }

    linkedlist->size -= 1;
    if(linkedlist->size == 0){
        free(l1->value);
        free(l1);
        linkedlist->head = NULL;
        return 1;
    }
    
    l1 = linkedlist->copy(l1->next);
    
    return 1;
}


/******************************
 * Other utility functions
 ******************************/

/* Get the size of the given set */
int linkedlist_size(LinkedList *linkedlist) {
    return linkedlist->size;
}

/* Print a representation of the linkedlist,
 * using the given function to print each
 * element
 */
void linkedlist_print(
        LinkedList *linkedlist,
        FILE *stream,
        void (*print_element)(FILE *, void *)) {
    fprintf(stream, "{size=%d} ", linkedlist->size);
    ListNode *bn = linkedlist->head;
    fprintf(stream, "[");
    while (bn) {
        print_element(stream, bn->value);
        bn = bn->next;
        if (bn) {
            fprintf(stream, ", ");
        }
    }
    fprintf(stream, "]");
}
