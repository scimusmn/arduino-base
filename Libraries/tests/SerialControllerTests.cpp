#include "tests.h"
#include "../SerialController.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * SerialCallback tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

bool success = false;
std::string str = "";
int integer = 0;
float number = 0.0f;

void voidcb() { success = true; }
void stringcb(const char *string) { str = string; }
void intcb(int n) { integer = n; }
void floatcb(float f) { number = f; }


mu_test scb_none() {
    smm::SerialCallback none;
    mu_assert_equal(none.valueType, smm::SerialCallback::ValueType::NONE);
    none("");
    return 0;
}


mu_test scb_void() {
    smm::SerialCallback v(voidcb);
    mu_assert_equal(v.callback.v, voidcb);
    v("literally anything");
    mu_assert_equal(success, true);
    return 0;
}


mu_test scb_string() {
    smm::SerialCallback s(stringcb);
    mu_assert_equal(s.callback.s, stringcb);
    s("hello!");
    mu_assert_equal(str, "hello!");
    return 0;
}


mu_test scb_int() {
    smm::SerialCallback i(intcb);
    mu_assert_equal(i.callback.i, intcb);
    i("42");
    mu_assert_equal(integer, 42);
    return 0;
}


mu_test scb_float() {
    smm::SerialCallback f(floatcb);
    mu_assert_equal(f.callback.f, floatcb);
    f("1.618f");
    mu_assert_equal(number, 1.618f);
    return 0;
}


mu_test scb_assignment() {
    success = false;
    
    smm::SerialCallback none;
    mu_assert_equal(none.valueType, smm::SerialCallback::ValueType::NONE);

    smm::SerialCallback v(voidcb);
    none = v;
    mu_assert_equal(none.callback.v, voidcb);
    mu_assert_equal(success, false);
    none("literally anything");
    mu_assert_equal(success, true);
    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * SerialController tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define SERIAL_RESET() do {			\
	Serial.reset();				\
	success = false;			\
	str = "";				\
	integer = 0;				\
	number = 0.0f;				\
    } while(0)


#define STANDARD_SETUP()				\
    smm::SerialController<> controller;			\
    mu_assert_equal(controller.maxNumCallbacks(), 16);	\
    mu_assert_equal(controller.numCallbacks(), 0);	\
    mu_assert_equal(controller.maxKeyLength(), 32);	\
    mu_assert_equal(controller.maxValueLength(), 32);	\
    controller.setup();					\
    controller.addCallback("void", voidcb);		\
    controller.addCallback("string", stringcb);		\
    controller.addCallback("int", intcb);		\
    controller.addCallback("float", floatcb);		\
    mu_assert_equal(controller.numCallbacks(), 4);


mu_test sctrl_normal() {
    STANDARD_SETUP();

    SERIAL_RESET();
    Serial.send("{void:1}");
    controller.update();
    mu_assert_equal(success, true);

    SERIAL_RESET();
    Serial.send("{string:hello}");
    controller.update();
    mu_assert_equal(str, "hello");

    SERIAL_RESET();
    Serial.send("{int:44}");
    controller.update();
    mu_assert_equal(integer, 44);

    SERIAL_RESET();
    Serial.send("{float:2.718}");
    controller.update();
    mu_assert_equal(number, 2.718f);

    return 0;
}


mu_test sctrl_overflow() {
    smm::SerialController<NORMAL_PROTOCOL, 2, 4> controller;

    mu_assert_equal(controller.maxNumCallbacks(), 2);
    mu_assert_equal(controller.numCallbacks(), 0);
    mu_assert_equal(controller.maxKeyLength(), 4);
    mu_assert_equal(controller.maxValueLength(), 4);

    controller.setup();
    controller.addCallback("void", voidcb);
    controller.addCallback("string", stringcb);
    controller.addCallback("int", intcb);
    controller.addCallback("float", floatcb);

    mu_assert_equal(controller.numCallbacks(), 2);

    SERIAL_RESET();
    Serial.send("{void:1}");
    controller.update();
    mu_assert_equal(success, true);

    SERIAL_RESET();
    Serial.send("{string:hello}");
    controller.update();
    mu_assert_unequal(str, "hello");
    mu_assert_equal(str, "");
    SERIAL_RESET();
    Serial.send("{stri:hello}");
    controller.update();
    mu_assert_equal(str, "hell");

    SERIAL_RESET();
    Serial.send("{int:44}");
    controller.update();
    mu_assert_unequal(integer, 44);

    SERIAL_RESET();
    Serial.send("{float:2.718}");
    controller.update();
    mu_assert_unequal(number, 2.718f);

    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * malformed input tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

mu_test sctrl_malformed_closingbracket() {
    STANDARD_SETUP();

    // controller should ignore messages with missing closing bracket
    SERIAL_RESET();
    Serial.send("{string:1");
    controller.update();
    mu_assert_equal(str, "");
    Serial.send("{string:b}");
    controller.update();
    mu_assert_unequal(str, "1{string:b");
    mu_assert_equal(str, "b");

    return 0;
}


mu_test sctrl_malformed_openingbracket() {
    STANDARD_SETUP();

    // controller should ignore messages with missing opening bracket
    SERIAL_RESET();
    Serial.send("string:1}");
    controller.update();
    mu_assert_equal(str, "");
    Serial.send("{string:b}");
    controller.update();
    mu_assert_equal(str, "b");

    return 0;
}


mu_test sctrl_malformed_colon() {
    STANDARD_SETUP();

    // controller should ignore messages with missing colon
    SERIAL_RESET();
    Serial.send("{string1}");
    controller.update();
    mu_assert_equal(str, "");
    Serial.send("{string:b}");
    controller.update();
    mu_assert_equal(str, "b");

    return 0;
}


mu_test sctrl_malformed_multi_closingbracket() {
    STANDARD_SETUP();

    // controller should process messages with multiple closing brackets
    SERIAL_RESET();
    Serial.send("{string:b}}}");
    controller.update();
    mu_assert_equal(str, "b");

    return 0;
}


mu_test sctrl_malformed_multi_openingbracket() {
    STANDARD_SETUP();

    // controller should process messages with multiple opening brackets
    SERIAL_RESET();
    Serial.send("{{{string:b}");
    controller.update();
    mu_assert_equal(str, "b");

    return 0;
}


mu_test sctrl_malformed_multi_colon() {
    STANDARD_SETUP();

    // controller should ignore messages with multiple colons
    SERIAL_RESET();
    Serial.send("{string:b:q:p}");
    controller.update();
    mu_assert_unequal(str, "b:q:p");
    mu_assert_equal(str, "");
    Serial.send("{string:b}");
    mu_assert_equal(str, "b");

    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialControllerTests() {
    int tests_run_old = tests_run;
    
    printf("running tests for SerialCallback\n");
    mu_run_test("create and call none callback", scb_none);
    mu_run_test("create and call void callback", scb_void);
    mu_run_test("create and call string callback", scb_string);
    mu_run_test("create and call integer callback", scb_int);
    mu_run_test("create and call float callback", scb_float);
    mu_run_test("assign callback to none callback", scb_assignment);
    printf("  ran %d tests\n", tests_run - tests_run_old);
    tests_run_old = tests_run;

    printf("running tests for SerialController\n");
    mu_run_test("normal operation", sctrl_normal);
    mu_run_test("callback and key/value overflow", sctrl_overflow);

    mu_run_test("ignore messages with no closing bracket", sctrl_malformed_closingbracket);
    mu_run_test("ignore messages with no opening bracket", sctrl_malformed_openingbracket);
    mu_run_test("ignore messages with no colon", sctrl_malformed_colon);
    mu_run_test("process messages with multiple closing brackets", sctrl_malformed_multi_closingbracket);
    mu_run_test("process messages with multiple opening brackets", sctrl_malformed_multi_openingbracket);
    mu_run_test("ignore messages with multiple colons", sctrl_malformed_multi_colon);
    printf("  ran %d tests\n", tests_run - tests_run_old);
}
