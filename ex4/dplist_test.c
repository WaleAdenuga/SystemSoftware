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
    asprintf(&new_name,"%s",((my_element_t*)element)->name);
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

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

void setup(void) {
    // Impl
    //test = dpl_create(element_copy, element_free, element_compare); 
}

void teardown(void) {
    // Implement post-test teardown
    //dpl_free(&test, true);
}
START_TEST(test_ListFree)
    {
        
    }
END_TEST

START_TEST(test_ListInsertion) {
    
}
END_TEST

START_TEST(test_ListSize) {
    
}
END_TEST

START_TEST(test_ListRemoval) {
    
}
END_TEST

START_TEST(test_ListElement) {
    
}
END_TEST

START_TEST(test_ListElementReference) {
    
}
END_TEST

int main(void) {
    Suite *s1 = suite_create("LIST_EX3");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListFree);
    tcase_add_test(tc1_1, test_ListElement);
    tcase_add_test(tc1_1, test_ListElementReference);
    tcase_add_test(tc1_1, test_ListRemoval);
    tcase_add_test(tc1_1, test_ListInsertion);
    tcase_add_test(tc1_1, test_ListSize);

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
