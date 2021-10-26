#include "tests/tests.h"
#include "Queue.h"


static mu_test queue_int();


void QueueTests() {
   int tests_run_old = tests_run;
    
   printf("running tests for Queue\n");

   mu_run_test("5-element integer queue", queue_int);
 
   printf("  ran %d tests\n", tests_run - tests_run_old);
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static mu_test queue_int() {
   smm::Queue<int, 5> queue;
   mu_assert_equal(queue.size(), 0);
   
   queue.push(5);
   queue.push(4);
   queue.push(3);
   queue.push(2);
   queue.push(1);

   mu_assert_equal(queue.size(), 5);
   
   mu_assert_equal(queue.front(), 5);
   queue.pop();
   mu_assert_equal(queue.front(), 4);
   queue.pop();
   mu_assert_equal(queue.front(), 3);
   queue.pop();
   mu_assert_equal(queue.front(), 2);
   queue.pop();
   mu_assert_equal(queue.front(), 1);

   return 0;
}
