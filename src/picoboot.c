/**
 * Copyright (c) 2025 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <stdio.h>
#include <tusb.h>

#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/structs/bus_ctrl.h"
#include "pico/stdlib.h"

#include "endian.h"
#include "hw.h"
#include "picoboot.pio.h"
#include "pio.h"
#include "status_led.h"
#include "version.h"

extern const uint32_t __payload[];
extern const uint32_t __payload_end[];

static const uint32_t payload_magic0 = 0x49504C42; // "IPLB"
static const uint32_t payload_magic1 = 0x4F4F5420; // "OOT "
static const uint32_t payload_magic2 = 0x5049434F; // "PICO"

static hw_board_type_t s_board_type;

size_t validate_payload() {
    if (BigEndian32(__payload[0]) != payload_magic0) {
        goto bad;
    }
    if (BigEndian32(__payload[1]) != payload_magic1) {
        goto bad;
    }

    size_t size = BigEndian32(__payload[2]) / sizeof(__payload[0]);
    size_t alignment = 1024 / sizeof(__payload[0]);
    size_t size_aligned = (size + alignment - 1) / alignment * alignment;

    if (&__payload[size_aligned] > __payload_end) {
        goto bad;
    }

    if (BigEndian32(__payload[size - 1]) != payload_magic2) {
        goto bad;
    }

    return size_aligned;

bad:
    return SIZE_MAX;
}

void main()
{
    stdio_init_all();
    adc_init();
    s_board_type = hw_detect_board_type();

    printf("PicoBoot (%s) by webhdx (c) 2025\n", FW_VER_STRING);
    printf("Board Type: %s\n", hw_board_type_to_string(s_board_type));

    size_t payload_size = validate_payload();
    if (payload_size == SIZE_MAX) {
        printf("PicoBoot: Invalid payload. Entering infinite loop.\n");
        status_led_init(s_board_type);

        while (true) {
            sleep_ms(500);
            status_led_toggle();
        }
    }

    // Set 250MHz clock to get more cycles in between CLK pulses.
    // This is the lowest value I was able to make the code work.
    // Should be still considered safe for most Pico boards.
    set_sys_clock_khz(250000, true);

    // Prioritize DMA engine as it does the most work
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    gpio_set_slew_rate(PIN_DI, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(PIN_DI, GPIO_DRIVE_STRENGTH_12MA);

    PIO pio = pio0;

    //
    // State Machine: Transfer Start
    //
    // Counts all consecutive transfers and sets IRQ 
    // when first 1 kilobyte transfer starts.
    //

    uint transfer_start_sm = pio_claim_unused_sm(pio, true);
    uint transfer_start_offset = pio_add_program(pio, &on_transfer_program);

    on_transfer_program_init(pio, transfer_start_sm, transfer_start_offset, PIN_CLK, PIN_CS, PIN_DI);

    pio_sm_put(pio, transfer_start_sm, (uint32_t) 224); // CS pulses
    pio_sm_exec(pio, transfer_start_sm, pio_encode_pull(true, true));
    pio_sm_exec(pio, transfer_start_sm, pio_encode_mov(pio_x, pio_osr));
    pio_sm_exec(pio, transfer_start_sm, pio_encode_out(pio_null, 32));

    //
    // State Machine: Clocked Output
    // 
    // It waits for IRQ signal from first SM and samples clock signal
    // to output IPL data bits.
    //

    uint clocked_output_sm = pio_claim_unused_sm(pio, true);
    uint clocked_output_offset = pio_add_program(pio, &clocked_output_program);

    clocked_output_program_init(pio, clocked_output_sm, clocked_output_offset, PIN_DI, PIN_CLK, PIN_CS);

    pio_sm_put(pio, clocked_output_sm, 8191); // 8192 bits, 1024 bytes, minus 1 because counting starts from 0 
    pio_sm_exec(pio, clocked_output_sm, pio_encode_pull(true, true));
    pio_sm_exec(pio, clocked_output_sm, pio_encode_mov(pio_y, pio_osr));
    pio_sm_exec(pio, clocked_output_sm, pio_encode_out(pio_null, 32));

    // Set up DMA for reading IPL to PIO FIFO
    int chan = dma_claim_unused_channel(true);

    dma_channel_config c = dma_channel_get_default_config(chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio, clocked_output_sm, true));
    channel_config_set_bswap(&c, true);

    dma_channel_configure(
        chan,
        &c,
        &pio->txf[clocked_output_sm],
        __payload,
        payload_size,
        true // start immediately
    );

    // Start PIO state machines
    pio_sm_set_enabled(pio, transfer_start_sm, true);
    pio_sm_set_enabled(pio, clocked_output_sm, true);

    printf("PicoBoot: Finished injecting payload. Entering infinite loop.\n");

    status_led_init(s_board_type);
    status_led_on();

    while (true) {
        tight_loop_contents();
    }
}
