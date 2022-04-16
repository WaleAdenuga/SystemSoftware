/**
 * \author Adewale Adenuga
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include "dplist.h"

//#define DEBUG
// not sure what this is for yet, okay it's for the makefile
//but pkg-config --cflags --libs check. make test does work so maybe it's not so important at the moment

/*
 * definition of error codes
 */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1   //error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2  //error due to a list operation applied on a NULL list

#ifdef DEBUG
#define DEBUG_PRINTF(...) 									                                        \
        do {											                                            \
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	    \
            fprintf(stderr,__VA_ARGS__);								                            \
            fflush(stderr);                                                                         \
                } while(0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition, err_code)                         \
    do {                                                                \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");      \
            assert(!(condition));                                       \
        } while(0)


/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    int number_elements;
};

/* int main() {
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 41, 0);
    list = dpl_insert_at_index(list, 44, 1);
    list = dpl_insert_at_index(list, 45, 2);
    list = dpl_insert_at_index(list, 47, 3);
    list = dpl_insert_at_index(list, 42, 4);

    list_print(list);

    list = dpl_remove_at_index(list, 1);
    list = dpl_remove_at_index(list, -15);

    list_print(list);
   
    return 0;
}
 */
void list_print(dplist_t *list) {
    dplist_node_t *nodal = list->head;
    while(nodal != NULL) {
        printf("%p <- Node(%p, %d) -> %p, index %d\n", nodal->prev, nodal, nodal->element, nodal->next, dpl_get_index_of_element(list, nodal->element));
        nodal = nodal->next;
        printf("%d\n", dpl_size(list));
    }
}

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    list->number_elements = 0;
    // pointer drawing breakpoint
    return list;
}

void dpl_free(dplist_t **list) {//pointer to pointer to list

    //TODO: add your code here
    //assert(*list != NULL);
    if ((*list) == NULL) {
        DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    } else {
        //This is essentially how you iterate over all the elements in the linked list
        dplist_t *test = *list; //that is assigning a pointer with a pointer to the actual list
        dplist_node_t *nodal = test ->head; //pointer to a particular node, this time the head
        //every list node of the list needs to be deleted
        while ((nodal) != NULL) 
        {
        dplist_node_t *next = nodal->next;//temporary dummy variable
        free(nodal);
        nodal = next;
        //test->head->prev = NULL;
        //test->head = nodal;
        }

    }
 
    free(*list); //list itself being deleted
    *list = NULL; //pointer to list set to NULL, let's avoid errors

}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/

dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t)); //allocate the new node in the heap 
    DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    list_node->element = element; //set its data to whatever needs to be stored
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1, adding a new item to the list for the first time
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node; //the head becomes the new node, but it's pointing to null anyways
        //list->number_elements++;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2, inserting at the start of the list
        list_node->prev = NULL; //previous of first node after head always points to null
        list_node->next = list->head; //setting the next pointer of the new node to the current first node of the list
        list->head->prev = list_node->next; //it could also just point to the node, since previous is already set to null
        list->head = list_node; //te head points to the new node, it's now the first in the list after the head
        //list->number_elements++;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL); //to be safe
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            //list->number_elements++;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            //list->number_elements++;
            // pointer drawing breakpoint
        }
    }
    list->number_elements++;
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    int count;
    dplist_node_t *current_node;
    if (list == NULL) return NULL;
    if (list->head == NULL) return list; //list empty, return unmodified list

    if (index <= 0) { //deleting the first element of the list
        list->head = list->head->next;
        list->head->prev = NULL;
        list->number_elements--;
        return list;
        //you'll probably have a memory leak if you run with valgrind here
    }

    else if (index >= dpl_size(list) || index == (dpl_size(list)-1)) { //deleting the last element of the list
        current_node = dpl_get_reference_at_index(list, (dpl_size(list)-1));
        dplist_node_t *node_before = current_node->prev; //one before becomes new end of the list
        node_before->next = NULL;
        free(current_node);
        list->number_elements--;
        return list;
    } 
    else if(index > 0 && index < dpl_size(list)) { 
            //deleting an element in the middle of the list for example
            current_node = dpl_get_reference_at_index(list,index);
            assert(current_node != NULL);
            current_node->prev->next = current_node->next;
            current_node->next->prev = current_node->prev;
            free(current_node);
            list->number_elements--;
            return list;
        }
    
    // free(current_node);
    // list->number_elements--; 
    // return list;

}

int dpl_size(dplist_t *list) {

    //TODO: add your code here
    if (list == NULL) {
        return -1;
    }
    if (list->head == NULL) return -1;
    return list->number_elements;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count;
    dplist_node_t *dummy;
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    if (list->head == NULL) return NULL;
    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++) {
        if (count >= index) return dummy;
    }
    return dummy;
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    int proceed = 0;
    element_t *element_at_index;
    dplist_node_t *dummy = list->head;
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    if(list == NULL) return 0; //case 4, list is null, return 0
    if(list->head == NULL) return 0; //case 3, list is empty, return 0
    if (index <= 0) { //element of the first node is returned
        return list->head->element; //the head pointer
    } else {
        if (index > dpl_size(list) || index == (dpl_size(list) -1)) { //returning the element at the end of the list
            while (dummy != NULL) {
            if (dummy->next == NULL) {
                return dummy->element;
                }
            dplist_node_t *next = dummy->next;
            dummy = next;
            }
        } else { 
            //returning the element for a node in between somewhere
            while(dummy->next != NULL) {
                if(proceed == index) {
                    return dummy->element;
                }
                proceed++;
                dummy = dummy->next;
            }
        }
    }


}

int dpl_get_index_of_element(dplist_t *list, element_t element) {

    //TODO: add your code here
    int count =0;
    if (list == NULL) return -1;
    if (list->head == NULL) return -1;
    dplist_node_t *nodal = list->head;
    while(nodal != NULL) {
        if (nodal->element == element) {
            return count;
        }
        count++;
        nodal = nodal->next;
    }
    return -1;
}



