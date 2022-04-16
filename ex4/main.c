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

void print_list(dplist_t *list);

//before you dereference a void pointer, it must be typecasted to appropriate pointer type.

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

int main() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    printf("%d\n", dpl_size(list));
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "malloc_fail"; //name should always be a string
    my_element_t *strolled = malloc(sizeof(my_element_t));
    strolled->id = '2';
    strolled->name = "warning";
    my_element_t *alonso = malloc(sizeof(my_element_t));
    alonso->id = '3';
    alonso->name = "Betsy";
    my_element_t *pot = malloc(sizeof(my_element_t));
    pot->id = '4';
    pot->name = "Saul Goodman";
//dpl_
   
    list = dpl_insert_at_index(list, element, 0, false);
    //printf("%d\n", dpl_size(list));
    //print_list(list);
    list = dpl_insert_at_index(list, strolled, 1, false);
    //printf("%d\n", dpl_size(list));
    //print_list(list);
    list = dpl_insert_at_index(list, alonso, 54, false);
    printf("%d\n", dpl_size(list));
    print_list(list);
    list = dpl_remove_at_index(list, -47, false);
    print_list(list);
    list = dpl_remove_at_index(list,1,true);
    printf("%d\n", dpl_size(list));
    print_list(list);
    free(element);
    free(pot);
    free(strolled);
    free(alonso);
    dpl_free(&list, false);
    printf("Mummy speaking facts\n");
    return 0;
}

void print_list(dplist_t *list) {
    for (int i = 0; i<dpl_size(list); i++) {
        printf("%p <- Node(%p, %p) -> %p, index %d\n", dpl_get_reference_at_index(list, (i-1)), dpl_get_reference_at_index(list, i), dpl_get_element_at_index(list, i), dpl_get_reference_at_index(list, (i+1)),  i);
        //printf("%d\n", dpl_size(list));
    
    }
}