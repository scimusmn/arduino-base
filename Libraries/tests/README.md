arduino-base Unit Tests
=======================

A set of unit tests using modified [minunit] testing macros.


Building & Running
------------------

In the `Libraries/tests` folder, on a *nix system run `./run_tests.sh`, or on Windows run `run_tests.bat`.


Writing Tests
-------------

Tests are just functions with signature `const char * (test)()`, but you can declare one as just `mu_test test()`, followed by the function body. Then, you can do whatever operations you want and add assertions using the minunit `mu_assert()` `mu_assert_equal()`, and `mu_assert_streq()` macros. All tests must `return 0` at the end.

### Example test

```c
mu_test example()
{
	int a = 5;
	mu_assert(5 > 4, "five is not greater than four!");
	mu_assert_equal(a, 5);
	
	const char *string = "hello, world!";
	mu_assert_streq(string, "hello, world!");
	
	return 0;
}
```


Project Organization
--------------------

Generally, there is a source file for each file in `Libraries/`, each defining a "test suite" function declared in `tests.h` and actually run in `tests.cpp`. When adding something new, it is advisable to create a new suite function, add it to `tests.c` and then define it in your new tests file.



[minunit]: https://web.archive.org/web/20140804170151/http://www.jera.com/techinfo/jtns/jtn002.html
