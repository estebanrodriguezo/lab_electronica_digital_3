#include "pico/stdlib.h"
#include "pico/binary_info.h"

const int pines_segmento[7] = {7, 6, 18, 19, 20, 2, 1};
const int pines_seleccion_display[6] = {15, 14, 13, 12, 11, 10};

const uint8_t patrones_digitos[10] = {
    0b0000001,  // 0
    0b1001111,  // 1
    0b0010010,  // 2
    0b0000110,  // 3
    0b1001100,  // 4
    0b0100100,  // 5
    0b0100000,  // 6
    0b0001111,  // 7
    0b0000000,  // 8
    0b0000100   // 9
};

void inicializar_pines_display() {
    for (int i = 0; i < 7; i++) {
        gpio_init(pines_segmento[i]);
        gpio_set_dir(pines_segmento[i], GPIO_OUT);
    }

    for (int i = 0; i < 6; i++) {
        gpio_init(pines_seleccion_display[i]);
        gpio_set_dir(pines_seleccion_display[i], GPIO_OUT);
    }
}

void mostrar_digito_display(uint8_t digito) {
    if (digito >= 0 && digito <= 9) {
        for (int i = 0; i < 7; i++) {
            gpio_put(pines_segmento[i], (patrones_digitos[digito] >> i) & 1);
        }
    }
}

int main() {
    stdio_init_all();
    inicializar_pines_display();

    while (1) {
        for (int display = 0; display < 6; display++) {
            for (int i = 0; i < 6; i++) {
                gpio_put(pines_seleccion_display[i], i == display ? 0 : 1);
            }

            mostrar_digito_display(display);
            sleep_ms(2);

            // Apaga el display actual antes de pasar al siguiente
            for (int i = 0; i < 7; i++) {
                gpio_put(pines_segmento[i], 0);
            }
        }
    }

    return 0;
}
