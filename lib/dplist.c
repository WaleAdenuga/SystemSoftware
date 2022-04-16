/**
 * \author Adewale Adenuga
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"
//#include <stdbool.h>

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 

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
    void *element;
};

struct dplist {
    dplist_node_t *head;
    int number_elements;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};

int dpl_get_index_of_reference(dplist_t *list, dplist_node_t *reference);
//void dpl_print(dplist_t *list);

dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list = NULL;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {

    //TODO: add your code here
    if(*list == NULL) {
        DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    } else {
        dplist_t *current_list = *list;
        dplist_node_t *current_node = current_list -> head;
        
        while (current_node != NULL) {
            
            dplist_node_t *next = current_node->next;
            if(free_element == true) {
                current_list->element_free(&(current_node->element));
            }
            free(current_node);
            current_node = next;
        }
    }
    free(*list);
    *list = NULL;
}

/* void dpl_print(dplist_t *list) {
    dplist_node_t *nodal = list->head;
    while(nodal != NULL) {
        printf("%p <- Node(%p, %p) -> %p, index %d\n", nodal->prev, nodal, nodal->element, nodal->next, dpl_get_index_of_element(list, nodal->element));
        nodal = nodal->next;
        //printf("%d\n", dpl_size(list));
    }
} */

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    //TODO: add your code here
    if(list == NULL) return NULL;
    dplist_node_t *reference_at_index = NULL;
    dplist_node_t *new_node = NULL;
    new_node = malloc(sizeof(dplist_node_t));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);

    //assert(new_node != NULL);
    if (list->head == NULL) {
        new_node->prev = NULL;
        new_node->next = NULL;
        list->head = new_node;
        list->number_elements++;
        if (insert_copy == true) {
        new_node->element = list->element_copy(element);
    } else {
        new_node->element = element;
    } 
        return list;
    }   
    else if (index <= 0) {
        new_node->prev = NULL;
        new_node->next = list->head;
        list->head->prev = new_node;//->next;
        list->head = new_node;
        list->number_elements++;
        if (insert_copy == true) {
        new_node->element = list->element_copy(element);
    } else {
        new_node->element = element;
    } 
        return list;

    }  else {
        reference_at_index = dpl_get_reference_at_index(list, index); //primarily for insertion in the middle of the list
        if (index >= dpl_size(list)) {
            //assert(reference_at_index ->next == NULL);
            new_node->next = NULL;
            new_node->prev = dpl_get_reference_at_index(list, dpl_size(list)-1);
            dpl_get_reference_at_index(list, dpl_size(list)-1)->next = new_node;
            list->number_elements++;
            if (insert_copy == true) {
        new_node->element = list->element_copy(element);
    } else {
        new_node->element = element;
    } 
            return list;

        } else {
            new_node->prev = reference_at_index->prev;
            new_node->next = reference_at_index;
            reference_at_index->prev->next = new_node;
            reference_at_index->prev = new_node;
            list->number_elements++;
            if (insert_copy == true) {
        new_node->element = list->element_copy(element);
    } else {
        new_node->element = element;
    } 
            return list;
        }
    }
    // list->number_elements++;
    return list;

}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {

    //TODO: add your code here
    if (list == NULL) return NULL;
    if (list->head == NULL) return list;
    dplist_node_t *ref_at_index = NULL, *ref_after = NULL, *ref_before=NULL;
    if (free_element == true) {
        list->element_free(dpl_get_element_at_index(list, index));
    }
    if (index <= 0) {
        ref_at_index = list->head;
        list->head->next->prev = NULL;
        list->head = list->head->next;
    } else {
        if (index > dpl_size(list) || index == (dpl_size(list) - 1)) {
            ref_at_index = dpl_get_reference_at_index(list, index);
            ref_before = ref_at_index->prev;
            ref_before->next = NULL;
            free(ref_at_index);
            list->number_elements--;
            return list;
        } else {
            ref_at_index = dpl_get_reference_at_index(list, index);
            ref_before = dpl_get_reference_at_index(list, (index-1));
            ref_after = dpl_get_reference_at_index(list, (index+1));
            assert(ref_after != NULL);
            assert(ref_before != NULL);
            assert(ref_at_index != NULL);
            ref_before->next = ref_after;
            ref_after->prev = ref_before;
            free(ref_at_index);
            list->number_elements--;
            return list;
        }
    }
    //free(ref_at_index);
    //list->number_elements--;
    return list; 

}

int dpl_size(dplist_t *list) {

    //TODO: add your code here
    if (list == NULL) return -1;
    if (list->head == NULL) return 0;

    return list->number_elements;

}

void *dpl_get_element_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    if (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    int count = 0;
    dplist_node_t *nodal = list->head;
    if (index <= 0) {
        return nodal->element;
    } else {
        if (index >= (dpl_size(list)-1)) {
            while (nodal != NULL) {
                if (nodal->next == NULL) {
                    return nodal->element;
                }
                nodal = nodal->next;
            }
        } else {
            while (nodal != NULL) {
                if (count == index) {
                    return nodal->element;
                }
                count++;
                nodal = nodal ->next;
            }

        }
    }
    return nodal->element;

}

int dpl_get_index_of_element(dplist_t *list, void *element) {

    //TODO: add your code here
    if (list == NULL) return -1;
    if (list->head == NULL) return -1;
    int index = 0;
    dplist_node_t *nodal = list->head;
    
    while (nodal != NULL) {
        if (list->element_compare(nodal->element, element) == 0) {
            return index;
        }
        index++;
        nodal = nodal->next;
    }
    return -1;

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    if (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    int count = 0;
    dplist_node_t *nodal = list->head;

    if (index <= 0) {
        return nodal;

    } else {
        if (index > dpl_size(list) || index == dpl_size(list) - 1) {
            while (nodal != NULL) {
                if (nodal ->next == NULL) {
                    return nodal;
                }
                nodal = nodal->next;
            }
        } else {
            while (nodal != NULL) {
                if (count == index) {
                    return nodal;
                }
                count++;
                nodal = nodal->next;
            }

        }
    }
    return nodal;

}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {

    //TODO: add your code here
    if (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    if (reference == NULL) return NULL;

    dplist_node_t *nodal = list->head;
    while (nodal != NULL) {
        if (nodal == reference) return nodal ->element;
        nodal = nodal -> next;
    }
    return NULL;

}

dplist_node_t *dpl_get_first_reference(dplist_t *list) {
    if (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    return dpl_get_reference_at_index(list, 0);
}

dplist_node_t *dpl_get_last_reference(dplist_t *list) {
    if (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    return dpl_get_reference_at_index(list, (dpl_size(list)-1));
}

dplist_node_t *dpl_get_next_reference(dplist_t *list, dplist_node_t *reference) {
    if (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    if (reference == NULL) return NULL;
    //if (reference->prev == NULL && reference->next == NULL) return NULL;
    dplist_node_t *next_reference = NULL;
    int index = dpl_get_index_of_reference(list, reference);
    if (index == -1) return NULL; //reference does not exist in the list
    else {
        next_reference = dpl_get_reference_at_index(list, (index+1));
    }
    return next_reference;
}

dplist_node_t *dpl_get_previous_reference(dplist_t *list, dplist_node_t *reference) {
    if (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    if (reference == NULL) return NULL;
    dplist_node_t *previous_reference = NULL;
    int index = dpl_get_index_of_reference(list, reference);
    if (index == -1) return NULL; //reference does not exist in the list
    else {
        previous_reference = dpl_get_reference_at_index(list, (index-1));
    }
    return previous_reference;
}

dplist_node_t *dpl_get_reference_of_element(dplist_t *list, void* element) {
    if (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    int current_index = 0;

    dplist_node_t *element_reference = NULL;
    void *element_at_index = NULL;
    for (int i = 0; i<dpl_size(list); i++) {
        element_at_index = dpl_get_element_at_index(list, i);
        int compare = list->element_compare(element_at_index, element);
        if (compare != 0) current_index++;
    }
    if (current_index >= (dpl_size(list))) {
        return NULL;
    } else {
        element_reference = dpl_get_reference_at_index(list, dpl_get_index_of_element(list, element));
    }
    //loop through list, compare each element, increment local variable to size of list, 
    //then return null if that variable does equal the size of the list, 
    //else get index of element and get reference at that index.
    return element_reference;
}

int dpl_get_index_of_reference(dplist_t *list, dplist_node_t *reference) {
    if (list == NULL) return -1;
    if (reference == NULL) return -1;
    if (list->head == NULL) return -1;

    int index_reference = 0;
    dplist_node_t *node_reference = NULL;
    for (int i = 0; i<dpl_size(list); i++) {
        node_reference = dpl_get_reference_at_index(list, i);
        index_reference++;
        if (node_reference == reference) return (index_reference-1);
    }
    if (index_reference >= dpl_size(list)) return -1;
    return index_reference;
}

dplist_t *dpl_insert_at_reference(dplist_t *list, void *element, dplist_node_t *reference, bool insert_copy) {
    if (list == NULL) return NULL;
    if (reference == NULL) return NULL;
    int index_at_reference = dpl_get_index_of_reference(list, reference);
    if (index_at_reference == -1) return list;
    if (insert_copy == true) list = dpl_insert_at_index(list, element, index_at_reference, true);
    else list = dpl_insert_at_index(list, element, index_at_reference, false);
    return list;
}

dplist_t *dpl_insert_sorted(dplist_t *list, void *element, bool insert_copy) {
    if (list == NULL) return NULL;
    if(list->head == NULL) return list;
    //you have to compare elements of different nodes without knowing the nodes themselves
    //or maybe instead of trying to switch nodes, try to switch elements
    bool listSorted = false;
    dplist_node_t *sydney = NULL, *sydeon = NULL;
    void *current_element = NULL;
    for (sydney = list->head; sydney->next !=NULL; sydney = sydney->next) {
        for (sydeon = sydney->next; sydeon != NULL; sydeon = sydeon->next) {
            int bad = list->element_compare(sydeon->element, sydney->element);
            if (bad == -1) {
                current_element = sydney->element;
                sydney->element = sydeon->element;
                sydeon->element = current_element;
            }
        }
        listSorted = true;
    }

    //insert to sorted, make it loop through everything, use element compare
    //when it notices one it is less than, it stops in that position
    //and inserts the new list node there
    int element_compare = 0;
    if (listSorted == true) {
    for (int i = 0; i<dpl_size(list); i++) {
        element_compare = list->element_compare(element, dpl_get_element_at_index(list, i));
        if (element_compare == -1 && insert_copy == true) {
            list = dpl_insert_at_index(list, element, i, true);
            return list;
        } 
        else if (element_compare == -1 && insert_copy == false) {
            list = dpl_insert_at_index(list, element,i, false);
            return list;
        } 
    }
    }
    return list;
}

dplist_t *dpl_remove_at_reference(dplist_t *list, dplist_node_t *reference, bool free_element) {
    if (list == NULL) return NULL;
    if (reference == NULL) return NULL;
    int index_at_reference = dpl_get_index_of_reference(list, reference);
    if (index_at_reference == -1) return list;
    if (free_element == true) list = dpl_remove_at_index(list, index_at_reference, true);
    else list = dpl_remove_at_index(list, index_at_reference, false);
    return list;
}

dplist_t *dpl_remove_element(dplist_t *list, void *element, bool free_element) {
    if (list == NULL) return NULL;
    int index_of_element = dpl_get_index_of_element(list, element);
    if (index_of_element == -1) return list;
    if (free_element == true) list = dpl_remove_at_index(list, index_of_element, true);
    else list = dpl_remove_at_index(list, index_of_element, false);
    return list;
}


