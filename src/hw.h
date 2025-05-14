/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef HW_H
#define HW_H

// Defines the possible board types
typedef enum {
    HW_BOARD_TYPE_PICO,
    HW_BOARD_TYPE_PICO_W,
    HW_BOARD_TYPE_PICO_2,
    HW_BOARD_TYPE_PICO_2_W,
    HW_BOARD_TYPE_UNKNOWN
} hw_board_type_t;

/**
 * @brief Detects the type of Raspberry Pi Pico board.
 * 
 * This function reads the ADC on GPIO29 (ADC3) to differentiate
 * between a Pico and a Pico W. Pico W typically shows a much lower
 * voltage on VSYS (connected to ADC3) when the wireless circuitry is present.
 * 
 * @note The ADC system must be initialized by calling adc_init() before this function.
 * 
 * @return hw_board_type_t The detected board type (HW_BOARD_TYPE_PICO or HW_BOARD_TYPE_PICO_W).
 */
hw_board_type_t hw_detect_board_type(void);

/**
 * @brief Converts a hw_board_type_t enum to its string representation.
 * 
 * @param board_type The board type enum.
 * 
 * @return const char* The string representation of the board type.
 */
const char* hw_board_type_to_string(hw_board_type_t board_type);

#endif // HW_H 