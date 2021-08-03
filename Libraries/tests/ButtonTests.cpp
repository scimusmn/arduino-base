#include "tests.h"
#include "../Button.h"

#define assert_up() mu_assert_equal(button.getState(), smm::BUTTON_UP)
#define assert_down() mu_assert_equal(button.getState(), smm::BUTTON_DOWN)

mu_test button_state() {
    smm::Button button(3);
    assert_up();
    
    digitalWrite(3, 0);
    button.update();
    assert_up();
    
    delay(100);
    button.update();
    assert_down();
    
    digitalWrite(3, 1);
    button.update();
    assert_down();
    
    delay(100);
    button.update();
    assert_up();
    
    return 0;
}


mu_test button_debounce_quickpress() {
    smm::Button button(3, 20);
    assert_up();

    digitalWrite(3, 0);
    button.update();
    assert_up();

    delay(2);
    button.update();
    assert_up();

    digitalWrite(3, 1);
    button.update();
    assert_up();

    delay(20);
    button.update();
    assert_up();

    return 0;
}


static bool triggered = false;
static void trigger() { triggered = true; }

#define press() do { digitalWrite(3, 0); button.update(); } while (0)
#define release() do { digitalWrite(3, 1); button.update(); } while (0)
#define pressFor(ms) do { press(); delay(ms); button.update(); release(); } while (0)
#define releaseFor(ms) do { release(); delay(ms); button.update(); press(); } while (0)

mu_test button_onpress() {
    smm::Button button(3);
    button.onPress(trigger);
    triggered = false;

    // do not trigger on brief pulse
    pressFor(3);
    mu_assert_equal(triggered, false);

    // trigger on longer pulse
    pressFor(30);
    mu_assert_equal(triggered, true);

    return 0;
}


mu_test button_onrelease() {
    smm::Button button(3);
    button.onRelease(trigger);
    triggered = false;

    press();
    delay(30);
    button.update();
    assert_down();

    // do not trigger on brief pulse
    releaseFor(3);
    mu_assert_equal(triggered, false);

    // trigger on longer pulse
    releaseFor(30);
    mu_assert_equal(triggered, true);

    return 0;
}


mu_test button_whiledown() {
    smm::Button button(3);
    button.whileDown(trigger);
    triggered = false;

    // do not trigger on brief pulse
    pressFor(3);
    mu_assert_equal(triggered, false);

    // trigger on longer pulse
    pressFor(30);
    mu_assert_equal(triggered, true);
    triggered = false;

    // while held, trigger even during shorter pulse
    releaseFor(3);
    mu_assert_equal(triggered, true);
    triggered = false;

    // stop triggering after long release
    releaseFor(30);
    release();
    triggered = false;
    button.update();
    mu_assert_equal(triggered, false);

    return 0;
}


mu_test button_whileup() {
    smm::Button button(3);
    button.whileUp(trigger);

    press();
    delay(30);
    button.update();
    triggered = false;

    // do not trigger on brief pulse
    releaseFor(3);
    mu_assert_equal(triggered, false);

    // trigger on longer pulse
    releaseFor(30);
    mu_assert_equal(triggered, true);
    triggered = false;

    // while released, trigger even during shorter pulse
    pressFor(3);
    mu_assert_equal(triggered, true);
    triggered = false;

    // stop triggering after long press
    pressFor(30);
    press();
    triggered = false;
    button.update();
    mu_assert_equal(triggered, false);

    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ButtonTests() {
    int tests_run_old = tests_run;
    printf("running tests for Button\n");

    mu_run_test("switching pin state", button_state);
    mu_run_test("debounce quick press", button_debounce_quickpress);
    mu_run_test("trigger onPress callback", button_onpress);
    mu_run_test("trigger onRelease callback", button_onrelease);
    mu_run_test("trigger whileDown callback", button_whiledown);
    mu_run_test("trigger whileUp callback", button_whileup);
    

    printf("  ran %d tests\n", tests_run - tests_run_old);
}
