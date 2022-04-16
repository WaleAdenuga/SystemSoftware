/**
 * \author Adewale Adenuga
 */

#include "dplist.h"
//#include "dplist.c"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void setup(void) {
    // Implement pre-test setup
}

void teardown(void) {
    // Implement post-test teardown
}
START_TEST(test_ListFree)
    {
        // Test free NULL
        dplist_t *list = NULL;
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list
        list = dpl_create();
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result at empty to be NULLg");
        // TODO : Test free with one element
        list = dpl_create();
        dplist_t *result2 = dpl_insert_at_index(list,47,0);
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failire: expected result with one to be NULL");

        // TODO : Test free with multiple element
        list = dpl_create();
        result2 = dpl_insert_at_index(list,47,0);
        result2 = dpl_insert_at_index(list,54,1);
        result2 = dpl_insert_at_index(list,61,2);
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result with multiple to be NULL");

    }
END_TEST

START_TEST(test_ListInsertAtIndexListNULL)
    {
        // Test inserting at index -1
        dplist_t *result = dpl_insert_at_index(NULL, 'A', -1);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        // TODO : Test inserting at index 0
        dplist_t *result2 = dpl_insert_at_index(NULL, 54, 0);
        ck_assert_msg(result2 == NULL, "Failure: expected list at index 0 to be NULL");
        // TODO : Test inserting at index 99
        dplist_t *index99 = dpl_insert_at_index(NULL, 96, 99);
        ck_assert_msg(index99 == NULL, "Failure: expected inseting at 99 to be NULL");
    }
END_TEST

START_TEST(test_ListInsertAtIndexListEmpty)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 'A', -1);
    ck_assert_msg(dpl_size(list) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(list));
    dpl_free(&list);
    // TODO : Test inserting at index 0
    dplist_t *creation = dpl_create();
    creation = dpl_insert_at_index(creation, 54, 0);
    int x;
    int *p = NULL;
    x = dpl_size(creation);
    p = &x;
    ck_assert_msg(*p == 1, "Failure: expected a size of 1 when inserting at index 0, got %d instead", *p);
    dpl_free(&creation);
    // TODO : Test inserting at index 99
    dplist_t *visual = dpl_create();
    visual = dpl_insert_at_index(visual, 21, 99);
    int z;
    int *y = NULL;
    z = dpl_size(visual);
    y = &z;
    ck_assert_msg(*y == 1, "Failure: expected a size of 1 when inserting at index 99");
    int index = dpl_get_index_of_element(visual, 21);
    ck_assert_msg(index == 0, "Failure, expected something different");
    dpl_free(&visual);

}
END_TEST


START_TEST(test_ListSize) {
    //Testing the size of the list
    dplist_t *list = dpl_create();
    int x;
    int *p = NULL;
    x= dpl_size(list);
    p = &x;
    ck_assert_msg(*p == -1, "Failure in size of list at initial creation");
    list = dpl_insert_at_index(list, 25, 0);
    list = dpl_insert_at_index(list, 55, 1);
    list = dpl_insert_at_index(list, 75, 2);
    list = dpl_insert_at_index(list, 61, 3);
    list = dpl_insert_at_index(list, 49, 4);
    list = dpl_insert_at_index(list, 54, 5);

    ck_assert_msg(dpl_size(list) == 6, "Failure:expected list to have size of 6, got a size of %d", dpl_size(list));
    list = dpl_remove_at_index(list, 0);
    list_print(list);
    list = dpl_remove_at_index(list, 4);
    list_print(list);
    ck_assert_msg(dpl_size(list) == 4, "Failure, expected a value of 4, got a value of %d instead", dpl_size(list));
    dpl_free(&list);
}
END_TEST



START_TEST(test_removeItemAtIndex) {
    //Testing removing a specific item at a specific index and seeing if everything remains normal
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 30, 0);
    list = dpl_insert_at_index(list, 23, 1);
    list = dpl_insert_at_index(list, 47, 2);
    list = dpl_insert_at_index(list, 100, 3);
    list = dpl_insert_at_index(list, 21, 4);
    list = dpl_insert_at_index(list, 46, 5);

    //list_print(list);
    
    //Now it's time to check that indexes point to their appropriate places, first with a negative index
    int l = -15;
    list = dpl_remove_at_index(list, l);
    //list_print(list);
    //printf("%d", dpl_size(list));
    ck_assert_msg(dpl_size(list) == 5, "Your remove operation does not account for negative numbers %d", dpl_size(list));
    ck_assert_msg((dpl_get_index_of_element(list, 21) == 3), "Check out your negative operation for remove");

    int g = 4;
    //list = dpl_remove_at_index(list, g);

    //idea here is to use character pointers to get the singular string in the node
    //writing the code in dplist.c will decide if it actually works or not

    //Testing insertion at a index greater than the size of the dplist created above
    int s = 1000;
    list = dpl_insert_at_index(list, 52, s);
    ck_assert_msg(dpl_size(list) == 6, "Failure, does not account for large indexes");
    ck_assert_msg(dpl_get_index_of_element(list, 52) == 5, "Expected addition at the back end of the list");
    list = dpl_remove_at_index(list,1000);
    ck_assert_msg(dpl_size(list) == 5, "Failure, still doesn't account for large indexes");
    dpl_free(&list);

}
END_TEST

START_TEST(test_ElementAtIndex) {
    //This is essentially the previous test but I didn't notice this function on time so.
    //Test if after insertion and deletion, element at new node is as expected.
    dplist_t *list = dpl_create();
    int x;
    int *p = NULL;
    x = dpl_get_index_of_element(list, 59);
    p = &x;
    ck_assert_msg(x == -1, "Failure, list was just created, there should be no element");
    list = dpl_insert_at_index(list, 54, 0);
    list = dpl_insert_at_index(list, 12, 1);
    list = dpl_insert_at_index(list, 45, 2);
    list = dpl_insert_at_index(list, 47, 3);
    list = dpl_insert_at_index(list, 95, 4);

    list = dpl_remove_at_index(list, 2);
    list_print(list);


    list = dpl_insert_at_index(list,84,4);
    list_print(list);
    element_t test = (dpl_get_element_at_index(list, 2));
    if (test != 47) {
        ck_abort_msg("You have absolutely failed the test");
    }

    //a test for comparing the actual elements in the list
    element_t test1 = dpl_get_element_at_index(list, 0);
    element_t test2 = dpl_get_element_at_index(list, 4);
    if (test1 != 54) {
        ck_abort_msg("Check again, your function about elements is wrong");
    }
    if (test2 != 84) {
        ck_abort_msg("Again, that element index function has issues %p", test2);
    }

    //Testing the appropriate index of the element after inputting a value not in the list
    ck_assert_msg(dpl_get_index_of_element(list, 49) == -1, "Element is not supposed to be in list");
    char o, g;
    char *k = NULL, *l = NULL;
    o = dpl_get_element_at_index(list, -20);
    g = dpl_get_element_at_index(list, 0);
    k = &o;
    l = &g;
    ck_assert_msg(o == g, "Your list is supposed to give the same result for negative values and zero");
    dpl_free(&list);
}
END_TEST

//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

int main(void) {
    Suite *s1 = suite_create("LIST_EX1");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListFree);
    tcase_add_test(tc1_1, test_ListInsertAtIndexListNULL);
    tcase_add_test(tc1_1, test_ListInsertAtIndexListEmpty);
    tcase_add_test(tc1_1, test_removeItemAtIndex);
    tcase_add_test(tc1_1, test_ListSize);
    //tcase_add_test(tc1_1, test_removingItemAtSpecificIndex);
    //tcase_add_test(tc1_1, test_ListSizeAfterAddition);
    tcase_add_test(tc1_1, test_ElementAtIndex);

    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
