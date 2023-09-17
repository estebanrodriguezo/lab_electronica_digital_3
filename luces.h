#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#define LED_AMARILLO 16  // GPIO 16 para el LED amarillo
#define LED_VERDE 17     // GPIO 17 para el LED verde
#define LED_ROJO 18      // GPIO 18 para el LED rojo

// Configurar pines GPIO para LEDs como salidas



/*
void encenderledamarillo(){
    gpio_put(LED_AMARILLO, 1);
}

void apagarledamarillo(){
    gpio_put(LED_AMARILLO, 0);
}

void parpadearledamarillo(){
    //Establecer funcion para parpadear a 1Hz

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);    // definir esto para que parpadee a 1HZ
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
}


void encenderledverde(){
    gpio_put(LED_VERDE, 1);
    //establecer tiempo de 10 segundos para apagado
}

void apagarledverde(){
    gpio_put(LED_VERDE, 0);
}


void encenderledrojo(){
    gpio_put(LED_ROJO, 1);
    sleep_ms(3000); 
    apagarledrojo();
    //establecer tiempo de 3 segundos para apagado
}

void apagarledrojo(){
    gpio_put(LED_ROJO, 0);
}


*/

