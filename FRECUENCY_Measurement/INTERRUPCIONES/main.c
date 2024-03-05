#include "hardware/irq.h"
#include <stdbool.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "math.h"
#include "pico/time.h"

#define PULSE_INPUT_PIN  22 //Pin de lectura de frecuencia
#define TIMER_INTERVAL 1000

typedef _Bool (*repeating_timer_callback_t)(struct repeating_timer *rt);

volatile uint32_t pulse_count = 0;
volatile uint32_t previous_pulse_count = 0;
volatile uint32_t elapsed_time = 0;
volatile uint32_t start_time = 0;

timer_t timer;
repeating_timer_t timer_id;

//                             g  f  e  d   c  b  a
const int pines_segmento[7] = {1, 2, 20, 19, 18, 6, 7};
const int pines_seleccion_display[6] = {15, 14, 13, 12, 11, 10};

// Define los patrones de segmentos para los dígitos 0-9

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

void pulse_isr() {
    pulse_count++;
}

float calcular_frecuencia(uint32_t pulse_count, uint32_t previous_pulse_count, uint32_t start_time) {
    uint32_t pulse_diff = pulse_count - previous_pulse_count;
    uint64_t elapsed_time = time_us_32() - start_time;
    float freq=(float)pulse_diff / (elapsed_time / 1e6);

    return freq;
}

void inicializar_pines_display() {
    for (int i = 0; i < 7; i++) {
        gpio_init(pines_segmento[i]);
        gpio_set_dir(pines_segmento[i], GPIO_OUT);
        gpio_put(pines_segmento[i], 1); // Apaga todos los segmentos
    }

    for (int i = 0; i < 6; i++) {
        gpio_init(pines_seleccion_display[i]);
        gpio_set_dir(pines_seleccion_display[i], GPIO_OUT);
        gpio_put(pines_seleccion_display[i], 0); // Apaga todos los displays
    }
}

void mostrar_digito_display(uint8_t digito) {
    if (digito >= 0 && digito <= 9) {
        for (int i = 0; i < 7; i++) {
            gpio_put(pines_segmento[i], (patrones_digitos[digito] >> i) & 0);
            //printf("aquiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii %d \n", digito);
        }
    }
}


void mostrar_frecuencia(float frequency) {
    int num = (int)(frequency);  // Redondea la frecuencia
    for (int i = 0; i < 6; i++) {
        int digito = (num / (int)pow(10, 5 - i)) % 10;
        printf("digito %i \n", digito);
        mostrar_digito_display(digito);
        

        // Apaga los segmentos antes de mostrar el siguiente dígito
        for (int j = 0; j < 7; j++) {
            gpio_put(pines_segmento[j], 0);
        }
    }
}





void mostrar_cadena_display(const char *cadena) {
    size_t longitud = sizeof(cadena);

    for (size_t i = 0; i < longitud; i++) {
        char caracter = cadena[i];
        if (caracter >= '0' && caracter <= '9') {
            uint8_t digito = caracter - '0'; // Convierte el carácter en un número
            for (int j = 0; j < 7; j++) {
                bool encendido = (patrones_digitos[digito] >> j) & 1;
                // Usa la variable 'encendido' para controlar el pin del segmento
                // gpio_put(pines_segmento[j], encendido);
            }
            printf("Carácter válido: %c\n", caracter);

        } else {
            // Carácter no válido, puedes manejar esto de acuerdo a tus necesidades
            printf("Carácter no válido: %c\n", caracter);
        }
    }
}


_Bool timer_isr(struct repeating_timer *rt) {
    
    float frequency = calcular_frecuencia(pulse_count, previous_pulse_count, start_time);


    // Creacion de variable auxiliar que ayuda a convertir el float, en una cadena de caracteres, esto con el fin de poder imprimir la temperatura en la LCD
    //char digits[6];
    //sprintf(digits, "%.2f", frequency);
    //int a = sizeof(digits);
    //for (int j = 0; j <a ; j++) {

    //        printf("Digitos: %c \n", digits[j]);

    //    }

    //mostrar_cadena_display(digits);
    mostrar_frecuencia(frequency);
    printf("Frecuencia: %.2f Hz\n", frequency);

    // Actualiza el valor anterior de conteo de pulsos
    previous_pulse_count = pulse_count;
    start_time = time_us_32();

    return true; 
}


int main() {
    stdio_init_all();
    inicializar_pines_display();

    gpio_init(PULSE_INPUT_PIN);
    gpio_set_dir(PULSE_INPUT_PIN, GPIO_IN);

    // Configura la interrupción para detectar flancos de subida
    gpio_set_irq_enabled_with_callback(PULSE_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &pulse_isr);
    add_repeating_timer_ms(TIMER_INTERVAL, timer_isr, NULL, &timer_id);


    while (1) {
    tight_loop_contents();
    }

    return 0;
}
