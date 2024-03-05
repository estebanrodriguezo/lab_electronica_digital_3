#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

#define PIN_BUTTON1 8  // Pin del primer pulsador
#define PIN_BUTTON2 11 // Pin del segundo pulsador
#define PIN_BUTTON3 14 // Pin del tercer pulsador

// Se declaran tres variables globales button1_pressed, button2_pressed y button3_pressed como volatile que se utilizarán para indicar si se ha presionado algún botón.
volatile bool button1_pressed = false;
volatile bool button2_pressed = false;
volatile bool button3_pressed = false;

// Se define una función llamada button_irq_handler que se ejecutará cuando ocurra una interrupción en cualquiera de los pines de los botones.
void button_irq_handler(uint gpio, uint32_t events)
{
    // se comprueba si el gpio corresponde a alguno de los pines de los botones y si el evento es un flanco de subida.
    if (gpio == PIN_BUTTON1 && (events & GPIO_IRQ_EDGE_RISE))
    {
        button1_pressed = true;
    }
    else if (gpio == PIN_BUTTON2 && (events & GPIO_IRQ_EDGE_RISE))
    {
        button2_pressed = true;
    }
    else if (gpio == PIN_BUTTON3 && (events & GPIO_IRQ_EDGE_RISE))
    {
        button3_pressed = true;
    }
}

// Se define una función llamada init_button que se utiliza para inicializar la configuración de un botón.
void init_button(uint pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_down(pin);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE, true, &button_irq_handler);
}