#include "tests.h"
#include "../LookupTable.h"

mu_test lut_create() {
    smm::LookupTable<smm::String32, int, 8> table;
    mu_assert_equal(table.size(), 0);
    mu_assert_equal(table.maxSize(), 8);
    return 0;
}


mu_test lut_add_entries() {
    smm::LookupTable<smm::String32, int, 8> table;
    mu_assert_equal(table.size(), 0);
    mu_assert_equal(table.maxSize(), 8);
    bool success = table.add("twenty", 20);
    mu_assert(success, "failed to add key 'twenty' to table!");
    success = table.add("five", 5);
    mu_assert(success, "failed to add key 'five' to table!");

    mu_assert_equal(table.size(), 2);
    mu_assert_equal(table.maxSize(), 8);
    return 0;
}


mu_test lut_lookup_entry() {
    smm::LookupTable<smm::String32, int> table;
    bool success = table.add("twenty", 20);
    mu_assert(success, "failed to add key 'twenty' to table!");
    success = table.add("five", 5);
    mu_assert(success, "failed to add key 'five' to table!");

    int *n = table["twenty"];
    mu_assert_unequal(n, nullptr);
    mu_assert_equal(*n, 20);
    n = table["five"];
    mu_assert_unequal(n, nullptr);
    mu_assert_equal(*n, 5);
    n = table["dne"];
    mu_assert_equal(n, nullptr);
    return 0;
}


mu_test lut_add_entries_overflow() {
    smm::LookupTable<smm::String32, int, 2> table;
    bool success;
    success = table.add("one", 1);
    mu_assert(success, "failed to add key 'one' to table!");
    success = table.add("two", 2);
    mu_assert(success, "failed to add key 'two' to table!");
    success = table.add("three", 3);
    mu_assert(!success, "incorrectly succeeded in adding key 'three' to table!");

    int *n;
    n = table["one"];
    mu_assert_unequal(n, nullptr);
    mu_assert_equal(*n, 1);

    n = table["two"];
    mu_assert_unequal(n, nullptr);
    mu_assert_equal(*n, 2);

    n = table["three"];
    mu_assert_equal(n, nullptr);
    
    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LookupTableTests() {
    int tests_run_old = tests_run;
    printf("running tests for LookupTable\n");

    mu_run_test("create LookupTable", lut_create);
    mu_run_test("add entries to table", lut_add_entries);
    mu_run_test("lookup table entries", lut_lookup_entry);
    mu_run_test("check table entry list overflow", lut_add_entries_overflow);
    printf("  ran %d tests\n", tests_run - tests_run_old);
}
