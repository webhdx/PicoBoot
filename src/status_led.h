/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef STATUS_LED_H
#define STATUS_LED_H

#include "hw.h"

void status_led_init(hw_board_type_t board_type);

void status_led_on(void);

void status_led_off(void);

void status_led_toggle(void);

#endif // STATUS_LED_H 
