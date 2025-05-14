/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef STATUS_LED_GPIO_DRIVER_H
#define STATUS_LED_GPIO_DRIVER_H

#include "pico/stdlib.h"

int status_led_gpio_init(void);

void status_led_gpio_on(void);

void status_led_gpio_off(void);

void status_led_gpio_toggle(void);

#endif 