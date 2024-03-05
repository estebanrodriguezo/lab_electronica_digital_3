#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <math.h>

#define SENSOR_PIN 28  // Puerto al que está conectado el LM35
#define SENSOR2_PIN 27 // Conexion al sensor de distancia

float read_temperature()
{
    adc_init();
    // Se inicializa el ADC y se configura el pin SENSOR2_PIN para su uso.
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(2);
    uint16_t temp_adc_value = adc_read();
    float voltaje = (temp_adc_value / 65535.0f) * 3.3f; // Convertir el valor ADC a voltaje
    // 65535 es el valor máximp que puede entregar el conversor ADC
    // raw_adc_value / 65535.0f convierte el valor digital a un valor en el rango de 0 a 1, representando la fracción de la escala máxima del ADC.
    // 3.3f es la referencia de voltaje utilizada para la conversión
    float temperatura_celsius = ((voltaje - 0.5f)*(-100.0f))-22; // Convertir voltaje a temperatura en grados Celsius
    // 0.5f se resta para corregir un posible offset en la lectura,
    // 100.0f se multiplica para escalar la diferencia de voltaje en grados Celsius, ya que el LM35 tiene una relación de 10mV/°C.
    return temperatura_celsius;
}

float read_distance()
{
    adc_init();
    // Se inicializa el ADC y se configura el pin SENSOR2_PIN para su uso.
    adc_gpio_init(SENSOR2_PIN);
    adc_select_input(1);
    uint16_t dis_adc_value = adc_read();
    // Convertir el valor ADC a voltaje
    float voltaje_dis = (dis_adc_value / 65535.0) * 5.0f;
    // Se calcula la distancia en centímetros a partir del voltaje medido.
    float distance_cm = 1.3 * pow(voltaje_dis, -1);

    return distance_cm;
}
