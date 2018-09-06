# description
This is a very easy to use C unit test frame
# usage
copy knova_unit.h and linux_kernel_list.h to your workspace
and write code like the style below:
``` C
#include "knova_unit.h"
int Foo() {
    // do something
    return 1;
}
void case1() {
    KNOVA_TEST_TRUE(Foo());
}
struct st {
    int a;
    int b;
};
void case2() {
    struct st st1 = {1,2}, st2 = {1,2};
    // note, memalign problem in compare struct
    KNOVA_TEST_STRUCT_EQUAL(struct st, &st1, &st2);
}
int Foo2() {
    // do something
    return 1;
}
void case3() {
    KNOVA_TEST_STR_EQUAL("HI","HI2");
}
void case4() {
    KNOVA_TEST_TRUE(Foo());
    KNOVA_TEST_TRUE(0);
}
int main(){
    knova_unit_init();

    knova_new_test("Foo");
    knova_add_case(case1);
    knova_add_case(case2);
    knova_new_test("Foo2");
    knova_add_case(case3);
    knova_add_case(case4);

    knova_test_run_all();
    
    return 0;
}
```

and after test runed, it will show infos like below:
```
Test Done!
totally [2]tests [4]cases runed.
totally [1]tests [2]cases failed.

======Fail test[1] [Foo2]:======
  case [1]:
     [1]:in file[./List_UnitTest.c] case3(), line: 22; ["HI"] and ["HI2"] suppose to be equal, but not
  case [2]:
     [1]:in file[./List_UnitTest.c] case4(), line: 26; [0] suppose to be true, but is false;
```