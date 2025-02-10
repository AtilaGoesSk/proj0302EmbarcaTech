#ifndef WS2812B_PIO_H
#define WS2812B_PIO_H

#include "hardware/pio.h"

void ws2812b_program_init(PIO pio, uint sm, uint offset, uint pin, float freq);

#endif // WS2812B_PIO_H