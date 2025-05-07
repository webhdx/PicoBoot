/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include "hardware/pio.h"

void on_transfer_program_init(PIO pio, uint sm, uint offset, uint clk_pin, uint cs_pin, uint out_pin);
void clocked_output_program_init(PIO pio, uint sm, uint offset, uint data_pin, uint clk_pin, uint cs_pin);
