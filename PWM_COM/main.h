#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
//#include "funciones.h"

const uint OUTPUT_PIN = 2;
const uint MEASURE_PIN = 5;

float measure_duty_cycle(uint gpio) {
    // Solo los pines PWM B se pueden usar como entradas.
    assert(pwm_gpio_to_channel(gpio) == PWM_CHAN_B);
    uint slice_num = pwm_gpio_to_slice_num(gpio);

    // Configurar la medición del ciclo de trabajo
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv(&cfg, 100);
    pwm_init(slice_num, &cfg, false);
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    pwm_set_enabled(slice_num, true);
    sleep_ms(10);
    pwm_set_enabled(slice_num, false);

    float counting_rate = clock_get_hz(clk_sys) / 100;
    float max_possible_count = counting_rate * 0.01;
    return pwm_get_counter(slice_num) / max_possible_count;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    char letra = 'A';

    while (1) {
        printf("Ingresa una letra (A-Z o a-z) desde el teclado: ");
        char input = getchar();

        if (input >= ' ' && input <= '}') {
            letra = input;
        } else {
            printf("Caracter no válido. Ingresa una letra en mayúscula (A-Z).\n");
            continue;
        }

        float valor_float = (float)(int)letra;
        if (valor_float < 0.0) valor_float = valor_float / (1000 * -1);
        if (valor_float > 1.0) valor_float = valor_float / 1000;

        printf("El valor ASCII de '%c' en float es: %.2f\n", letra, valor_float);

        const uint count_top = 1000;
        pwm_config cfg = pwm_get_default_config();
        pwm_config_set_wrap(&cfg, count_top);
        pwm_init(pwm_gpio_to_slice_num(OUTPUT_PIN), &cfg, true);

        float output_duty_cycle_n = valor_float;
        pwm_set_gpio_level(OUTPUT_PIN, (uint16_t)(output_duty_cycle_n * (count_top + 1)));
            
       
        float measured_duty_cycle = measure_duty_cycle(MEASURE_PIN);
        printf("Output duty cycle = %.1f%%, measured input duty cycle = %.1f%%\n",
               output_duty_cycle_n * 100.f, measured_duty_cycle * 100.f);
        int output_pin_value = gpio_get(OUTPUT_PIN);
        printf("Valor del pin OUTPUT_PIN: %d\n", output_pin_value);
        
        gpio_set_function(OUTPUT_PIN, GPIO_FUNC_PWM);
        
        sleep_ms(1000);
    }

    return 0;
}
