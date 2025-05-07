/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <stdbool.h>
#include <stddef.h>

#include "hw.h"
#include "status_led.h"
#include "status_led/cyw43.h"
#include "status_led/gpio.h"

typedef int (*led_init_func_p)(void);
typedef void (*led_op_func_p)(void);

typedef struct {
    led_init_func_p init;
    led_op_func_p on;
    led_op_func_p off;
    led_op_func_p toggle;
} status_led_driver_t;

static int led_init_noop(void) {
    return 0;
}

static void led_op_noop(void) {
    // Do nothing
}

static status_led_driver_t s_led_driver = {
    .init = led_init_noop,
    .on = led_op_noop,
    .off = led_op_noop,
    .toggle = led_op_noop,
};

void status_led_init(hw_board_type_t board_type) {
    if (board_type == HW_BOARD_TYPE_PICO_W || board_type == HW_BOARD_TYPE_PICO_2_W) {
        s_led_driver.init = status_led_cyw43_init;
        s_led_driver.on = status_led_cyw43_on;
        s_led_driver.off = status_led_cyw43_off;
        s_led_driver.toggle = status_led_cyw43_toggle;
    } else {
        s_led_driver.init = status_led_gpio_init;
        s_led_driver.on = status_led_gpio_on;
        s_led_driver.off = status_led_gpio_off;
        s_led_driver.toggle = status_led_gpio_toggle;
    }

    if (s_led_driver.init() != 0) {
        s_led_driver.init = led_init_noop;
        s_led_driver.on = led_op_noop;
        s_led_driver.off = led_op_noop;
        s_led_driver.toggle = led_op_noop;
    }
}

void status_led_on(void) {
    s_led_driver.on();
}

void status_led_off(void) {
    s_led_driver.off();
}

void status_led_toggle(void) {
    s_led_driver.toggle();
}
