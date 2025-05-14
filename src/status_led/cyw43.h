/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef STATUS_LED_CYW43_DRIVER_H
#define STATUS_LED_CYW43_DRIVER_H

int status_led_cyw43_init(void);

void status_led_cyw43_on(void);

void status_led_cyw43_off(void);

void status_led_cyw43_toggle(void);

#endif 