// Inclusão de bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812b.pio.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"



// Definições de hardware
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define LEDS_COUNT 25
#define LEDS_PIN 7
#define BRIGHTNESS 0.2

// Definições de botões e LEDs
#define RED_LED 13
#define GREEN_LED 11
#define BLUE_LED 12
#define BUTTON_A 5
#define BUTTON_B 6

// Definição da estrutura para os LEDs (ordem GRB)
typedef struct {
    uint8_t g, r, b;
} pixel_t;

pixel_t leds[LEDS_COUNT]; // Matriz de LEDs

// Buffer para armazenar a entrada do usuário
char input_buffer[32];
int input_index = 0;

// Variáveis globais para PIO e state machine
static PIO np_pio;
static uint sm;
ssd1306_t ssd; // Instância do display
bool cor = true; // Cor de fundo do display

// Variáveis globais para controle dos botões
volatile uint32_t tempo_anterior = 0;

// Inicializa a máquina de estado PIO
void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2812b_program);
    np_pio = pio0;

    sm = pio_claim_unused_sm(np_pio, true);
    ws2812b_program_init(np_pio, sm, offset, pin, 800000.f);

    pio_sm_set_enabled(np_pio, sm, true);

    // Desliga todos os LEDs ao iniciar
    for (uint i = 0; i < LEDS_COUNT; ++i) {
        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;
    }
}

// Função para enviar dados para os LEDs
void send_to_leds() {
    for (uint i = 0; i < LEDS_COUNT; ++i) {
        uint32_t pixel_color = ((uint32_t)leds[i].g << 16) | ((uint32_t)leds[i].r << 8) | ((uint32_t)leds[i].b);
        pio_sm_put_blocking(np_pio, sm, pixel_color);
    }
}

// Converte índices da matriz 5x5 para LEDs WS2812B (invertendo verticalmente)
int getIndex(int x, int y) {
    if (y % 2 == 0) {
        return (4 - y) * 5 + (4 - x);  // Espelhamos a linha par
    } else {
        return (4 - y) * 5 + x;  // Mantemos a linha ímpar normal
    }
}


// Exibe um número na matriz de LEDs
void display_number(uint8_t number) {
    // Matriz de números (0-9)
    const int numbers_animation[11][5][5][3] = {
        // 0
        {
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},     
            {{255, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 0, 0}},
            {{255, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 0, 0}},
            {{255, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 0, 0}},
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}}      
        },
        
        {
            {{0, 0, 0}, {0, 0, 0}, {230, 42, 0}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {230, 42, 0}, {230, 42, 0}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {0, 0, 0}, {230, 42, 0}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {0, 0, 0}, {230, 42, 0}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {230, 42, 0}, {230, 42, 0}, {230, 42, 0}, {0, 0, 0}}       
        },
        
        {
            {{204, 85, 0}, {204, 85, 0}, {204, 85, 0}, {204, 85, 0}, {204, 85, 0}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {204, 85, 0}},
            {{204, 85, 0}, {204, 85, 0}, {204, 85, 0}, {204, 85, 0}, {204, 85, 0}},
            {{204, 85, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
            {{204, 85, 0}, {204, 85, 0}, {204, 85, 0}, {204, 85, 0}, {204, 85, 0}}
        },
        
        {
            {{255, 128, 0}, {255, 128, 0}, {255, 128, 0}, {255, 128, 0}, {255, 128, 0}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 128, 0}},
            {{0, 0, 0}, {255, 128, 0}, {255, 128, 0}, {255, 128, 0}, {255, 128, 0}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 128, 0}},
            {{255, 128, 0}, {255, 128, 0}, {255, 128, 0}, {255, 128, 0}, {255, 128, 0}}
        },
        
        {
            {{255, 170, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 170, 0}},
            {{255, 170, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 170, 0}},
            {{255, 170, 0}, {255, 170, 0}, {255, 170, 0}, {255, 170, 0}, {255, 170, 0}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 170, 0}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 170, 0}}
        },

        // ... continuar com a definição dos outros números até o 9 ...
    };

    // Desenho do número selecionado no display de LEDs
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            int idx = getIndex(x, y);
            leds[idx].r = numbers_animation[number][y][x][0];
            leds[idx].g = numbers_animation[number][y][x][1];
            leds[idx].b = numbers_animation[number][y][x][2];
        }
    }

    send_to_leds();
}

int main() {
    // Inicializações
    stdio_init_all();
    npInit(LEDS_PIN);

    // Configuração do display I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, I2C_PORT, endereco);
    ssd1306_clear(&ssd);

    // Exemplo de exibição do número 5 na matriz
    display_number(5);
    
    while (true) {
        tight_loop_contents();
    }

    return 0;
}
