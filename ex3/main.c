/**
 * \author Adewale Adenuga
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include "dplist.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

dplist_t *test = NULL;

typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name); //implicit malloc, writes the space for you, and would avoind over and underflows
    
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

//before you dereference a void pointer, it must be typecasted to appropriate pointer type.

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

int main() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "malloc_fail"; //name should always be a string
    void *tester;
   
    list = dpl_insert_at_index(list, element, 1, true);
    void *test = dpl_get_element_at_index(list, 0);
    printf("%p\n", tester);
    //printf("%p\n", element);
    //printf("%p\n", ((my_element_t*)*element));
    dpl_print(list);
    //printf("\n%d", dpl_get_index_of_element(list, element));
    printf("%d\n", element_compare(element, tester));
    printf("%p\n", (void*)element_copy(element));
    //element_free(&tester);
    dpl_free(&list, true);
    //element_free(dpl_get_element_at_index(list, 0));
    //dpl_print(list);



    dplist_t *result = dpl_create(element_copy, element_free, element_compare);
    my_element_t *headphone = malloc(sizeof(my_element_t));
    headphone->id = 1;
    headphone->name = "Don Lemon";

    result = dpl_insert_at_index(list, headphone, 0, false);
    dplist_node_t *test_reference = dpl_get_reference_at_index(result, 0);
    if (test_reference == (dpl_get_reference_at_index(result, 0))) {
        printf("This is how you compare nodes");
    } else {
        printf("This is not how you compare nodes bro");
    }
    return 0;
}