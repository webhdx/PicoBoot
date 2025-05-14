/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "gpio.h"
#ifdef PICO_RP2040
#include "boards/pico.h"
#else
#include "boards/pico2.h"
#endif

static bool led_state_gpio = false;

int status_led_gpio_init(void) {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    led_state_gpio = false;

    return 0;
}

void status_led_gpio_on(void) {
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    
    led_state_gpio = true;
}

void status_led_gpio_off(void) {
    gpio_put(PICO_DEFAULT_LED_PIN, false);

    led_state_gpio = false;
}

void status_led_gpio_toggle(void) {
    if (led_state_gpio) {
        status_led_gpio_off();
    } else {
        status_led_gpio_on();
    }
}
