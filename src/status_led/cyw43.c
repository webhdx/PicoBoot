/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <stdbool.h>

#include "pico/cyw43_arch.h"
#ifdef PICO_RP2040
#include "boards/pico_w.h"
#else
#include "boards/pico2_w.h"
#endif

#include "cyw43.h"

static bool led_state_cyw43 = false;

int status_led_cyw43_init(void) {
    if (cyw43_arch_init()) {
        return 1; 
    }

    led_state_cyw43 = false;

    return 0;
}

void status_led_cyw43_on(void) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);

    led_state_cyw43 = true;
}

void status_led_cyw43_off(void) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);

    led_state_cyw43 = false;
}

void status_led_cyw43_toggle(void) {
    if (led_state_cyw43) {
        status_led_cyw43_off();
    } else {
        status_led_cyw43_on();
    }
} 