#include "tests.h"
#include "../LookupTable.h"

mu_test lut_create() {
    smm::LookupTable<8, int> table;
    mu_assert_equal(table.size(), 0);
    mu_assert_equal(table.maxSize(), 8);
    return 0;
}


mu_test lut_add_entries() {
    smm::LookupTable<8, int> table;
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


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LookupTableTests() {
    printf("running tests for LookupTable\n");

    mu_run_test("create LookupTable", lut_create);
    mu_run_test("add entries to table", lut_add_entries);
}
