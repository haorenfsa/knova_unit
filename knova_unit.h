#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linux_kernel_list.h"
/* Global type Definitions to be used for boolean operators. */
#ifndef BOOL
  /** Boolean type for CUnit use. */
  #define BOOL int
#endif

#ifndef TRUE
  /** Boolean TRUE for CUnit use. */
  #define TRUE 1
#endif

#ifndef FALSE
  /** Boolean FALSE for CUnit use. */
  #define FALSE	0
#endif


// configurable
#define FUNCTION_NAME_LEN 64
#define ERROR_MSG_LEN 256
//#define dprintf printf
#define dprintf(...) 

typedef struct TestCase TestCase_t;
typedef struct TestSet TestSet_t;
struct TestSet {
    list_head_t set_list_node;
    list_head_t fail_set_node;
    char name[FUNCTION_NAME_LEN];
    list_head_t case_list;
    list_head_t failcase_list;
    int count;
    int fail;
};

struct GlobalDatas{
    list_head_t set_list;
    list_head_t failset_list;
    TestSet_t* current_set;
    TestCase_t* current_case;
    int count; // set count
    int fail; // fail sets count
    BOOL cur_case_failed;
    BOOL cur_set_failed;
} k_global_data;

typedef void (*TEST_CASE) ();
struct TestCase {
    list_head_t case_list_node;
    list_head_t fail_case_node;
    list_head_t failmsg_list;
    TEST_CASE case_func;
    int count;
    int fail;
};

#define TYPE_INT 0
#define TYPE_STR 1
typedef struct FailMsg FailMsg_t;
struct FailMsg {
    list_head_t failmsg_node;
    char msg[256];
};

// functions start with k_ are internal utils
// start with knova_ are unit test apis
void k_generate_errormsg(){

}

struct GlobalDatas* k_pGlobalData;
void knova_unit_init() {
    k_pGlobalData = &k_global_data;
    LIST_HEAD_INIT2(&k_pGlobalData->set_list);
    LIST_HEAD_INIT2(&k_pGlobalData->failset_list);
}

void knova_new_test(char* name) {
    TestSet_t* new_set = (TestSet_t*)calloc(sizeof(TestSet_t), 1);
    dprintf("set_add:%p\n", new_set);
    dprintf("head_set: %p\n", &k_pGlobalData->set_list);
    LIST_HEAD_INIT2(&new_set->case_list);
    LIST_HEAD_INIT2(&new_set->failcase_list);
    strcpy(new_set->name, name);
    dprintf("pos_set: %p\n", &new_set->set_list_node);
    list_add_tail(&new_set->set_list_node, &k_pGlobalData->set_list);
    dprintf("pos_set: %p\n", k_pGlobalData->set_list.next);
    k_pGlobalData->current_set = new_set;
    k_pGlobalData->count++;
    return;
}

void knova_add_case(TEST_CASE case_func) {
    TestCase_t* new_case = (TestCase_t*)calloc(sizeof(TestCase_t), 1);
    dprintf("set_case:%p\n", new_case);
    list_add_tail(&new_case->case_list_node, &k_pGlobalData->current_set->case_list);
    k_pGlobalData->current_set->count++;
    new_case->case_func = case_func;
    LIST_HEAD_INIT2(&new_case->failmsg_list);
    k_pGlobalData->current_case = new_case;
    return;
}

#define KNOVA_TEST_TRUE(in) knova_test_true(in, #in, __FILE__, __FUNCTION__, __LINE__)
#define KNOVA_TEST_FALSE(in) knova_test_false(in, #in, __FILE__, __FUNCTION__, __LINE__)
#define KNOVA_TEST_STR_EQUAL(a,b) knova_test_str_equal(a, #a, b, #b, __FILE__, __FUNCTION__, __LINE__)
#define KNOVA_TEST_STRUCT_EQUAL(type, a, b) knova_test_struct_equal(sizeof(type), a, #a, b, #b, __FILE__, __FUNCTION__, ___LINE___)
void knova_test_true(BOOL in, char* in_str, const char* file, const char* func, int line) {
    if(in != TRUE) {
        k_pGlobalData->cur_case_failed = TRUE;
        FailMsg_t* new_msg = (FailMsg_t*)calloc(sizeof(FailMsg_t), 1);
        sprintf(new_msg->msg, "in file[%s] %s(), line: %d; [%s] suppose to be true, but is false;", file, func, line, in_str);
        list_add_tail(&new_msg->failmsg_node, &k_pGlobalData->current_case->failmsg_list);
    }
};

void knova_test_false(BOOL in, char* in_str, const char* file, const char* func, int line) {
    if(in != FALSE) {
        k_pGlobalData->cur_case_failed = TRUE;
        FailMsg_t* new_msg = (FailMsg_t*)calloc(sizeof(FailMsg_t), 1);
        sprintf(new_msg->msg, "in file[%s] %s(), line: %d; [%s] suppose to be false, but is true;", file, func, line, in_str);
        list_add_tail(&new_msg->failmsg_node, &k_pGlobalData->current_case->failmsg_list);
    }
};

BOOL k_test_str_equal(const char* a, const char* b) {
    if(a == NULL) {
        return a == b;
    }
    while(*a != '\0') {
        if(*a == *b) {
            a++;
            b++;
        }
        else {
            return FALSE;
        }
    }
    return *a == *b;
};

void knova_test_str_equal(const char* a, const char* b,
 char* a_str, char* b_str, const char* file, const char* func, int line) {
    if(k_test_str_equal(a,b)) {
        k_pGlobalData->cur_case_failed = TRUE;
        FailMsg_t* new_msg = (FailMsg_t*)calloc(sizeof(FailMsg_t), 1);
        sprintf(new_msg->msg, "in file[%s] %s(), line: %d; [%s] and [%s] suppose to be equal, but not", file, func, line, a_str, b_str);
        list_add_tail(&new_msg->failmsg_node, &k_pGlobalData->current_case->failmsg_list);
    }
}

BOOL k_test_struct_equal(int size, void* a, void* b) {
    return memcmp(a, b, size);
}
void knova_test_struct_equal(int size, void* a, void* b,
 char* a_str, char* b_str, const char* file, const char* func, int line) {
     if(k_test_struct_equal(size, a, b)) {
        FailMsg_t* new_msg = (FailMsg_t*)calloc(sizeof(FailMsg_t), 1);
        sprintf(new_msg->msg, "in file[%s] %s(), line: %d; [%s] and [%s] suppose to be equal, but not", file, func, line, a_str, b_str);
        list_add_tail(&new_msg->failmsg_node, &k_pGlobalData->current_case->failmsg_list);
    }
}

void knova_test_run_all() {

    TestSet_t* cur_set;
    TestCase_t* cur_case;
    list_head_t* head_set = &k_pGlobalData->set_list, *pos_set, *pos_case;
    dprintf("head_set: %p\n", head_set);
    dprintf("pos_set: %p\n", head_set->next);
    list_head_t* head_case;
    list_for_each(pos_set, head_set) {
        dprintf("pos_set: %p\n", pos_set);
        cur_set = list_entry(pos_set, TestSet_t, set_list_node);
        dprintf("set:%p a\n", cur_set);
        k_pGlobalData->current_set = cur_set;
        list_head_t* head_case = &cur_set->case_list;
        list_for_each(pos_case, head_case) {
            cur_case = list_entry(pos_case, TestCase_t, case_list_node);
            dprintf("cur_case:%p\n", cur_case);
            k_pGlobalData->current_case = cur_case;
            cur_case->case_func();
            if(k_pGlobalData->cur_case_failed) {
                cur_case->fail++;
                list_add_tail(&cur_case->fail_case_node, &cur_set->failcase_list);
                k_pGlobalData->cur_case_failed = FALSE;
                if (!k_pGlobalData->cur_set_failed) {
                    k_pGlobalData->cur_set_failed = TRUE;
                    list_add_tail(&cur_set->fail_set_node, &k_pGlobalData->failset_list);
                }
                k_pGlobalData->fail++;
            }
        }
        k_pGlobalData->cur_set_failed = FALSE;
    }
    // set loop
    printf("Test Done!");
    printf("totally [%d] test runed.\n", k_pGlobalData->count);
    if (k_pGlobalData->fail == 0) {
        printf("all pased!");
        return;
    }
    else {
        printf("totally [%d] cases failed.\n", k_pGlobalData->fail);
        int no_failset = 0, no_failcase = 0;
        head_set = &k_pGlobalData->failset_list;
        list_head_t* head_failmsg, *pos_failmsg;
        FailMsg_t* cur_failmsg;
        list_for_each(pos_set, head_set) {
            no_failset++;
            cur_set = list_entry(pos_set, TestSet_t, fail_set_node);
            printf("\n======Fail set[%d] [%s]:======\n", no_failset, cur_set->name);
            head_case = &cur_set->failcase_list;
            no_failcase = 0;
            list_for_each(pos_case, head_case) {
                no_failcase++;
                printf("  case [%d]:\n", no_failcase);
                cur_case = list_entry(pos_case, TestCase_t, fail_case_node);
                head_failmsg = &cur_case->failmsg_list;
                int i = 0;
                list_for_each(pos_failmsg, head_failmsg) {
                    cur_failmsg = list_entry(pos_failmsg, FailMsg_t, failmsg_node);
                    i++;
                    printf("     [%d]:%s\n", i, cur_failmsg->msg);
                }
            }
        }
    }
};