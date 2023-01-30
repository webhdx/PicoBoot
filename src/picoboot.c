/**
 * Copyright (c) 2022 Maciej Kobus
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "picoboot.pio.h"
#include "ws2812.pio.h"
#include "ipl.h"

const uint PIN_LED = 16;                // Status LED
const uint PIN_DATA_BASE = 6;           // Base pin used for output, 4 consecutive pins are used 
const uint PIN_CS = 4;                 // U10 chip select
const uint PIN_CLK = 5;                // EXI bus clock line
const uint DEFAULT_CLOCK = 125000;

uint32_t pixel_color = 0;
uint8_t pixel_brightness = 255;

/* Simple ws2812 implementation from https://github.com/ForsakenNGS/Pico_WS2812 */
void init_ws2812(PIO pio, uint sm)
{
    uint offset = pio_add_program(pio, &ws2812_program);
    const uint bits = 24; // RGB
    ws2812_program_init(pio, sm, offset, PIN_LED, 800000, bits);
}

void hsv_to_rgb(uint8_t hue, uint8_t saturation, uint8_t value, uint8_t* red, uint8_t* green, uint8_t* blue)
{
    if (saturation == 0) 
    {
        *red = 0;
        *green = 0;
        *blue = 0;
        return;
    }
    
    uint8_t quadrant = hue / 43;
    uint8_t remainder = (hue - (quadrant * 43)) * 6;
    uint8_t p = (value * (255 - saturation)) >> 8;
    uint8_t q = (value * (255 - ((saturation * remainder) >> 8))) >> 8;
    uint8_t t = (value * (255 - ((saturation * (255 - remainder)) >> 8))) >> 8;
    switch (quadrant) {
        case 0:
            *red = value;
            *green = t;
            *blue = p;
            return;
        case 1:
            *red = q;
            *green = value;
            *blue = p;
            return;
        case 2:
            *red = p;
            *green = value;
            *blue = t;
            return;
        case 3:
            *red = p;
            *green = q;
            *blue = value;
            return;
        case 4:
            *red = t;
            *green = p;
            *blue = value;
            return;
        default:
        case 5:
            *red = value;
            *green = p;
            *blue = q;
            return;
    }
}

void set_ws2812_brightness(uint8_t brightness)
{
    pixel_brightness = brightness;
}

void set_ws2812_color(uint8_t r, uint8_t g, uint8_t b)
{
    // GRBW
    g = (g * pixel_brightness) >> 8;
    r = (r * pixel_brightness) >> 8;
    b = (b * pixel_brightness) >> 8;
    pixel_color = (g << 24 | r << 16 | b << 8) & 0xFFFFFF00;
}

void show_ws2812(PIO pio, uint sm)
{
    pio_sm_put_blocking(pio, sm, pixel_color);
}

int main()
{
    // Set 250MHz clock to get more cycles in between CLK pulses.
    // This is the lowest value I was able to make the code work.
    // Should be still considered safe for most Pico boards.
    //printf("Set system clock to 250MHz\n");
    set_sys_clock_khz(250000, true);

    // Initialize and light up builtin LED, it will basically
    // act as a power LED.
    init_ws2812(pio1, 0);
    set_ws2812_brightness(64);

    uint8_t r = 255, g = 0, b = 0;
    set_ws2812_color(r, g, b);
    show_ws2812(pio1, 0);

   

    // Prioritize DMA engine as it does the most work
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    gpio_set_slew_rate(PIN_DATA_BASE, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(PIN_DATA_BASE + 1, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(PIN_DATA_BASE + 2, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(PIN_DATA_BASE + 3, GPIO_SLEW_RATE_FAST);

    gpio_set_drive_strength(PIN_DATA_BASE, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_drive_strength(PIN_DATA_BASE + 1, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_drive_strength(PIN_DATA_BASE + 2, GPIO_DRIVE_STRENGTH_4MA);
    gpio_set_drive_strength(PIN_DATA_BASE + 3, GPIO_DRIVE_STRENGTH_4MA);

    PIO pio = pio0;

    //
    // State Machine: Transfer Start
    //
    // Counts all consecutive transfers and sets IRQ 
    // when first 1 kilobyte transfer starts.
    //

    uint transfer_start_sm = pio_claim_unused_sm(pio, true);
    uint transfer_start_offset = pio_add_program(pio, &on_transfer_program);

    on_transfer_program_init(pio, transfer_start_sm, transfer_start_offset, PIN_CLK, PIN_CS, PIN_DATA_BASE);

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

    clocked_output_program_init(pio, clocked_output_sm, clocked_output_offset, PIN_DATA_BASE, PIN_CLK, PIN_CS);

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

    dma_channel_configure(
        chan,
        &c,
        &pio->txf[clocked_output_sm],
        ipl,
        count_of(ipl),
        true // start immediately
    );

    // Start PIO state machines
    pio_sm_set_enabled(pio, transfer_start_sm, true);
    pio_sm_set_enabled(pio, clocked_output_sm, true);

    // Wait for the DMA to finish.
    dma_channel_wait_for_finish_blocking(chan);

    // lower clock to normal
    set_sys_clock_khz(DEFAULT_CLOCK, true);

    // Successful DMAed, bring up the LED wheel
    uint8_t hue = 0;
    while(true)
    {
        tight_loop_contents();

        hsv_to_rgb(hue, 255, 255, &r, &g, &b);
        set_ws2812_color(r, g, b);
        show_ws2812(pio1, 0);

        sleep_ms(10);
        hue += 1;
    }

    return 0;
}
