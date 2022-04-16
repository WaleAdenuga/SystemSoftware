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
        // Test free NULL, don't use callback
        dplist_t *list = NULL;
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free NULL, use callback
        list = NULL;
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, don't use callback
         list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // TODO : Test free with one element, also test if inserted elements are set to NULL
        list = dpl_create(element_copy, element_free, element_compare);
        char a = 'C';
        void *l = &a;
        list = dpl_insert_at_index(list, l, 0, false);
        dpl_free(&list, true);
        ck_assert_msg(dpl_get_element_at_index(list, 0) == NULL, "Element should be freed");
        ck_assert_msg(list == NULL, "Failure, expected list to be NULL"); 
        // TODO : Test free with multiple element, also test if inserted elements are set to NULL

    }
END_TEST

START_TEST(test_ListInsertion) {
    //check insertion when the list is null
    dplist_t *list = NULL;
    ck_assert_msg(list == NULL, "Failure, your list should be NULL");

    //check normal insertion, make 2 copies, make 2 actual additions.
    list = dpl_create(element_copy,element_free,element_compare);
    char a, b, d, g;
    void *k, *l, *m, *h;
    a = 54; b = 41; d = 'J'; g = 5.5;
    k = &a; l = &b; m = &d; h = &g;
    
    list = dpl_insert_at_index(list, k, 0, false);
    list = dpl_insert_at_index(list, l, 1, true);
    list = dpl_insert_at_index(list, m, 2, false);
    list = dpl_insert_at_index(list, h, 3, true);

    ck_assert_msg(dpl_size(list) == 3, "Check again, the size of your list is incorrect");

    //check negative or zero insertion and insertion at the end of the list
    char ar, hk, lu, io, jk;
    void *gh, *fg, *bc, *bv, *nd;
    ar = 't'; hk = 'o'; lu = 10; io = 'i'; jk = 'c';
    gh = &ar; fg = &hk; bc = &lu; bv = &io; nd = &jk;
    dplist_t *negative = dpl_create(element_copy, element_free, element_compare);
    negative = dpl_insert_at_index(negative, gh, -45, false);
    negative = dpl_insert_at_index(negative, fg, 150, true);
    negative = dpl_insert_at_index(negative, bc, 0, false);
    negative = dpl_insert_at_index(negative, bv, 0, false);
    negative = dpl_insert_at_index(negative, nd, 1, true);

    ck_assert_msg(dpl_size(negative) == 5, "Your code for negative and back insertion does not work");
    ck_assert_msg(dpl_get_index_of_element(negative, fg) == 4, "Check again brother");
    ck_assert_msg(dpl_get_index_of_element(negative, bv) == 0, "Check again brother for zero insertion");
    ck_assert_msg(dpl_get_index_of_element(negative, nd) == 1, "Check again brother for normal insertion");
    dpl_free(&list, true);
    dpl_free(&negative, false);
}
END_TEST

START_TEST(test_ListSize) {
    dplist_t *list = NULL;
    ck_assert_msg(list == NULL, "Your list should be NULL");
    ck_assert_msg(dpl_size(list) == -1, "At creation, a null list should return -1");
    
    list = dpl_create(element_copy, element_free, element_compare);
    char a, b, d, g, mf, sc;
    void *k, *l, *m, *h, *hg, *kl;
    a = 54; b = 41; d = 'M'; g = 5.5; mf = 'B'; sc = 'C';
    k = &a; l = &b; m = &d; h = &g; hg = &mf; kl = &sc;
    list = dpl_insert_at_index(list, k, 0, false);
    list = dpl_insert_at_index(list, l, 1, true);
    list = dpl_insert_at_index(list, m, 2, false);
    list = dpl_insert_at_index(list, h, 3, true);
    ck_assert_msg(dpl_size(list) == 3, "Check again, the size of your list is incorrect");

    list = dpl_remove_at_index(list, -54, true);
    ck_assert_msg(dpl_size(list) == 3, "Your removal for negative numbers does not work");
    list = dpl_insert_at_index(list, hg, 456, false);
    ck_assert_msg(dpl_size(list) == 4, "Your list does not account for over the top addition");
    ck_assert_msg(dpl_get_index_of_element(list, hg) == 3, "Again, check for back addition");
    list = dpl_insert_at_index(list, kl, -541, true);
    ck_assert_msg(dpl_get_index_of_element(list, kl) == 0, "Doesn't account for negative addition to list");
    dpl_free(&list, false);
}
END_TEST

START_TEST(test_ListRemoval) {
    dplist_t *list = NULL;
    ck_assert_msg(list == NULL, "Your list should be NULL");
    ck_assert_msg(dpl_size(list) == -1, "At creation, a null list should return -1");
    
    list = dpl_create(element_copy, element_free, element_compare);
    char a, b, d, g, mf, sc;
    void *k, *l, *m, *h, *hg, *kl;
    a = 54; b = 41; d = 45; g = 5.5; mf = 74; sc = 'S';
    k = &a; l = &b; m = &d; h = &g; hg = &mf; kl = &sc;
    list = dpl_insert_at_index(list, k, 0, false);
    list = dpl_insert_at_index(list, l, 1, true);
    list = dpl_insert_at_index(list, m, 2, false);
    list = dpl_insert_at_index(list, h, 3, true);
    ck_assert_msg(dpl_size(list) == 4, "Check again, the size of your list is incorrect");

    list = dpl_remove_at_index(list, -54, true);
    ck_assert_msg(dpl_size(list) == 3, "Your removal for negative numbers does not work");
    ck_assert_msg(dpl_get_index_of_element(list, l) == 0, "Doesnt account for negative removal");
    list = dpl_remove_at_index(list, 456, false);
    ck_assert_msg(dpl_size(list) == 2, "Your list does not account for over the top addition");
    ck_assert_msg(dpl_get_index_of_element(list, m) == 1, "Again, check for back addition");
    list = dpl_insert_at_index(list, kl, -541, true);
    list = dpl_insert_at_index(list, h, 210, true);
    list = dpl_insert_at_index(list, m, 1, true);
    list = dpl_insert_at_index(list, hg, 2, false);
    ck_assert_msg(dpl_get_index_of_element(list, kl) == 0, "Doesn't account for negative addition to list");
    ck_assert_msg(dpl_get_index_of_element(list, hg) == 2, "Again, check for back addition");
    list = dpl_remove_at_index(list, 2, true);
    list = dpl_remove_at_index(list, 1, true);
    ck_assert_msg(dpl_size(list) == 4, "Removal at normal indexes do not work");
    ck_assert_msg(dpl_get_index_of_element(list, m) == 1, "CHECK");
    ck_assert_msg((dpl_get_element_at_index(list, (dpl_size(list)-1)) == h), "Your final element should be reviewed");
    dpl_free(&list, false);
}
END_TEST

START_TEST(test_ListElement) {
    dplist_t *list = NULL;
    ck_assert_msg(list == NULL, "Your list should be NULL");
    ck_assert_msg(dpl_size(list) == -1, "At creation, a null list should return -1");
    
    list = dpl_create(element_copy, element_free, element_compare);
    char a, b, d, g, mf, sc;
    void *k, *l, *m, *h, *hg, *kl;
    a = 54; b = 41; d = 12; g = 5.5; mf = 'I'; sc = 14;
    k = &a; l = &b; m = &d; h = &g; hg = &mf; kl = &sc;
    list = dpl_insert_at_index(list, k, 0, false);
    list = dpl_insert_at_index(list, l, 1, true);
    list = dpl_insert_at_index(list, m, 2, false);
    list = dpl_insert_at_index(list, h, 3, true);
    ck_assert_msg(dpl_size(list) == 4, "Check again, the size of your list is incorrect");
    ck_assert_msg(dpl_get_element_at_index(list, dpl_size(list)-1) == h, "Check out how specific your insertion is");
    ck_assert_msg(dpl_get_element_at_index(list, 2) == m, "Check out normal element retrieval");
    list = dpl_remove_at_index(list, -14, true);
    ck_assert_msg(dpl_get_element_at_index(list, 0) == l, "Your negative removal does work");
    list = dpl_insert_at_index(list, m, 450, true);
    ck_assert_msg(dpl_get_element_at_index(list, 120) == m, "Check out over the top insertion and retrieval");
    ck_assert_msg(dpl_get_element_at_index(list, -56) == l, "Check out negative beginnings");

    dplist_t *empty = NULL;
    ck_assert_msg(dpl_get_element_at_index(list, 5) == NULL, "Failure, expected list to be NULL");
    empty = dpl_create(element_copy, element_free, element_compare);
    ck_assert_msg(dpl_get_element_at_index(list, 4) == NULL, "List should be NULL");
    dpl_free(&list, false);
}
END_TEST

START_TEST(test_ListElementReference) {
    dplist_t *list = NULL;
    dplist_node_t *reference = NULL, *reference1, *reference2, *reference3, *reference4;
    ck_assert_msg(list == NULL, "Failure, list should be null");
    ck_assert_msg(dpl_get_element_at_reference(list, reference) == NULL, "A NULL reference should give a null element");
    
    list = dpl_create(element_copy, element_free, element_compare);
    ck_assert_msg(dpl_get_element_at_reference(list, reference2) == NULL, "Empty list, expected null reference");
    char a, b, c, d, e;
    void *k, *l, *m, *n, *o;
    a = 54; b = 'f'; c = 'g'; d= 'w'; e = 'a';
    k = &a; l = &b; m = &c; n = &d; o = &e;
    list = dpl_insert_at_index(list, k, 0, false);
    list = dpl_insert_at_index(list, l, 1, false);
    list = dpl_insert_at_index(list, m, 2, false);
    reference = dpl_get_reference_at_index(list, 0);
    reference1 = dpl_get_reference_at_index(list, 1);
    reference2 = dpl_get_reference_at_index(list, 2);
    ck_assert_msg(dpl_get_reference_at_index(list, 1) == reference1, "Failed comparison");
    ck_assert_msg(dpl_get_element_at_reference(list, reference2) == m, "Comparison of pointers failure");
    dpl_free(&list, true);
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
