#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
//#include "funciones.h"

//const uint OUTPUT_PIN = 2;
//const uint MEASURE_PIN = 5;
char letra;
bool is_transmitting = true; // Comienza transmitiendo
volatile bool signal_received = false;

#define UART_ID uart1
#define BAUD_RATE 9600
#define TX_PIN 4
#define RX_PIN 5

void clear_uart_buffer(uart_inst_t *uart) {
    while (uart_is_readable(uart)) {
        char c = uart_getc(uart);
    }
}

void configure_pwm() {
    // Configura el pin de transmisión (TX_PIN) como PWM
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv(&cfg, 100);
    pwm_init(pwm_gpio_to_slice_num(TX_PIN), &cfg, false);
    gpio_set_function(TX_PIN, GPIO_FUNC_PWM);
}

char measure_pwm_and_convert_to_char(uint gpio) {
    //printf("Entré a measurement");
    // Mide el ciclo de trabajo PWM y devuelve el valor
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
    float ciclo_dureza = ((pwm_get_counter(slice_num) / max_possible_count)+ 0.0002)*1000;
    printf("Ciclo recibido= %f.5 \n",ciclo_dureza);
    char char_recibido = (char)((int)ciclo_dureza);

    return char_recibido;
}

void convert_duty_cycle(char letra){
      
     if (letra >= ' ' && letra <= '}' && letra != 'a') {
        // Convierte el carácter a un valor de PWM (0-1)
        float valor_float = (float)(int)letra;
        if (valor_float < 0.0) valor_float = valor_float / (1000 * -1);
        if (valor_float > 1.0) valor_float = valor_float / 1000;

        const uint count_top = 1000;
        pwm_config cfg = pwm_get_default_config();
        pwm_config_set_wrap(&cfg, count_top);
        pwm_init(pwm_gpio_to_slice_num(TX_PIN), &cfg, true);

        float output_duty_cycle_n = valor_float;
        printf("Ciclo de dureza = %.6f \n", output_duty_cycle_n);
        pwm_set_gpio_level(TX_PIN, (uint16_t)(output_duty_cycle_n * (count_top + 1)));
        gpio_set_function(TX_PIN, GPIO_FUNC_PWM);
        sleep_ms(10);
        pwm_set_gpio_level(TX_PIN, 0);
        //gpio_set_function(TX_PIN, GPIO_FUNC_SIO);
     }
     else if(letra == 'a'){
          printf("Transmisión terminada... \n");
          printf("Recibiendo...\n");
            float valor_float = (float)(int)letra;
            if (valor_float < 0.0) valor_float = valor_float / (1000 * -1);
            if (valor_float > 1.0) valor_float = valor_float / 1000;

            const uint count_top = 1000;
           pwm_config cfg = pwm_get_default_config();
           pwm_config_set_wrap(&cfg, count_top);
           //pwm_init(pwm_gpio_to_slice_num(TX_PIN), &cfg, true);

            
            float output_duty_cycle_n = valor_float;
            printf("Ciclo de dureza = %.6f \n", output_duty_cycle_n);
            pwm_set_gpio_level(TX_PIN, (uint16_t)(output_duty_cycle_n * (count_top + 1)));
            gpio_set_function(TX_PIN, GPIO_FUNC_PWM);
            is_transmitting = false;
            clear_uart_buffer(UART_ID);
            sleep_ms(10);
            pwm_set_gpio_level(TX_PIN, 0);

            
            

          
     }
}


void gpio_isr_handler() {
    signal_received = true;
}


int main() {
    stdio_init_all();

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(TX_PIN, GPIO_FUNC_PWM);
    gpio_set_function(RX_PIN, GPIO_FUNC_PWM);
    configure_pwm();

    gpio_set_irq_enabled_with_callback(RX_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_isr_handler);

    char input_char;
    char received_message[256]; // Almacena el mensaje recibido
    int received_message_length = 0; // Longitud actual del mensaje recibido

    while (1) {
       
    if (is_transmitting == false) {
       
        if (signal_received){
            char received_char = measure_pwm_and_convert_to_char(RX_PIN);
            printf("Recibiste: %c \n", received_char);

            if (received_char == 'a') {

                printf("Mensaje recibido: %s\n", received_message);
                printf("Transmitiendo... \n");
                received_message[0] = '\0'; // Reiniciar el mensaje
                received_message_length = 0;
                is_transmitting = true;

            } else {
                received_message[received_message_length] = received_char;
                received_message[received_message_length + 1] = '\0';
                received_message_length++;
            }
            signal_received = false;
        }
    }
        
    if (is_transmitting == true ) {
            if (uart_is_writable(UART_ID)) {
                //printf("Ingresa una letra (A-Z o a-z) desde el teclado: ");
                input_char = getchar();
                
                if (input_char != '\n') {
                    printf("Enviaste: %c\n", input_char);
                    convert_duty_cycle(input_char);
                }
            }
        }
    }
    return 0;
}