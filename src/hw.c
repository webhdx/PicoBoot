/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "pico/platform.h"

#include "hw.h"

#define ADC_PIN_VSYS 29
#define ADC_INPUT_VSYS 3

// Threshold for differentiating Pico from Pico W based on raw ADC reading.
// Pico W typically has a much lower reading on VSYS ADC (e.g., ~0x01c).
// Pico typically has a higher reading (e.g., ~0x2cd).
// A value around 0x100 should be a safe threshold.
#define PICO_W_ADC_THRESHOLD 0x100

hw_board_type_t hw_detect_board_type(void) {
    adc_gpio_init(ADC_PIN_VSYS);
    adc_select_input(ADC_INPUT_VSYS);

    uint16_t adc_result = adc_read();

    if (adc_result < PICO_W_ADC_THRESHOLD) {
        #if defined(PICO_RP2350)
        return HW_BOARD_TYPE_PICO_2_W;
        #else
        return HW_BOARD_TYPE_PICO_W;
        #endif
    } else {
        #if defined(PICO_RP2350)
        return HW_BOARD_TYPE_PICO_2;
        #else
        return HW_BOARD_TYPE_PICO;
        #endif
    }
}

const char* hw_board_type_to_string(hw_board_type_t board_type) {
    switch (board_type) {
        case HW_BOARD_TYPE_PICO:
            return "Raspberry Pi Pico";
        case HW_BOARD_TYPE_PICO_W:
            return "Raspberry Pi Pico W";
        case HW_BOARD_TYPE_PICO_2:
            return "Raspberry Pi Pico 2";
        case HW_BOARD_TYPE_PICO_2_W:
            return "Raspberry Pi Pico 2 W";
        default:
            return "Unknown";
    }
}
