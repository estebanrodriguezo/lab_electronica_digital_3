/**
 * @file main.c
 * @version 1.0
 * @brief Archivo principal en el que se leen los elementos y se aplican las funciones definidas en pantalla_lcd.h
 */
//--------------------------------------------------------------------------------------
//------- DATA LOGGER ------------------------------------------------------------------
//------- Uso de Raspberry Pi Pico (GPIO) ----------------------------------------------
//------- Acondicionar una señal análoga y su conversión a digital-----------------------
//------- Por: Esteban Rodriguez Ordoñez - esteban.rodriguezo@udea.edu.co --------------
//------- Maria Salome Garces Montero - msalome.garces@udea.edu.co ---------------------
//------- Facultad de Ingeniería - Departamento de Electrónica y Telecomunicaciones ----
//------- Electronica Digital III ------------------------------------------------------
//------- Septiembre de 2023 -----------------------------------------------------------
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//-- 1. Importar bibliotecas -----------------------------------------------------------
//--------------------------------------------------------------------------------------
#include "lcd.h"
#include "senales.h"
#include "pulsadores.h"

// Función toma un arreglo de números de punto flotante y su tamaño como entrada, suma todos los elementos en el arreglo
//  y devuelve el promedio de esos valores como un número de punto flotante.
float calculate_average(float *samples, int num_samples)
{
   float sum = 0.0;
   for (int i = 0; i < num_samples; i++)
   {
      sum += samples[i];
   }
   return sum / num_samples;
}

/**********************************************************************/
/*                        Funcion Princupal                           */
/**********************************************************************/
int main()
{

   // Inicializacion de las funciones de entrada y salida estándar
   stdio_init_all();

   // Se iniciale=iza cada uno de los pulsadores
   init_button(PIN_BUTTON1);
   init_button(PIN_BUTTON2);
   init_button(PIN_BUTTON3);

   // Se declaran varias variables que se utilizarán en el programa

   int frequency = 1000;             // Inicializar con 1000 Hz
   int period_ms = 1000 / frequency; // Periodo
   int num_samples = 8;              // numero de muestras inicial

   bool show_temperature = false;

   // Variables que almacenan los valores promedio de las senales
   float average_value_tem = 0;
   float average_value_dis = 0;
   // Tiempo que se espera para garantizar inicializacion de la LCD
   sleep_ms(5000);
   // Inicializacion de la LCD
   init(RS, EN, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7);

   while (1)
   {

      // Condicional que se asegura de elegir que senal analoga sera usada
      if (button1_pressed)
      {
         show_temperature = !show_temperature; // Alternar entre mostrar temperatura y distancia
         button1_pressed = false;
      }

      // Si la variable show_temperature es verdadera, se calcula la temperatura en grados Celsius utilizando una función llamada read_temperature.
      if (show_temperature)
      {
         // Extraccion de la temperartura de la raspberry pi pico, usando la funcion read_temperature
         float temperature_celsius = read_temperature();
         // Creacion de variable auxiliar que ayuda a convertir el float, en una cadena de caracteres, esto con el fin de poder imprimir la temperatura en la LCD
         char buffer[10];
         sprintf(buffer, "%.2f", temperature_celsius);

         // Escribir el valor de la Temperatura en la LCD
         write_string("T: ");
         write_string(buffer);
         write_string("  C");

         // Escribir el valor de la Temperatura y Frecuencia de muestreo en la terminal
         printf("Temperatura: %.2f°C\n", temperature_celsius);
         printf("Frecuencia: %d Hz\n", frequency);

         // Se crea un array llamado samples para almacenar varias muestras de temperatura.
         float samples[num_samples];
         // Bucle for para obtener num_samples muestras de temperatura y almacenarlas en el array samples.
         for (int i = 0; i < num_samples; i++)
         {
            samples[i] = read_temperature();
         }
         // Se calcula el valor promedio de estas muestras utilizando la función calculate_average
         float average_value_tem = calculate_average(samples, num_samples);

         char buffer4[10];
         sprintf(buffer4, "%.2f", average_value_tem);

         write_string("\nProm: ");
         write_string(buffer4);
         write_string(" C");

         printf("Valor promedio: %.2f\n", average_value_tem, " °C");
         sleep_ms(150);
         clear_display();
      }

      // Si la variable show_temperature es falsa, se calcula la distancia en centimetros utilizando una función llamada read_distance.
      else if (show_temperature == false)
      {
         // Extraccion de la distancia de la raspberry pi pico, usando la funcion read_distance
         float distancia = read_distance();

         // Creacion de variable auxiliar que ayuda a convertir el float, en una cadena de caracteres, esto con el fin de poder imprimir la distancia en la LCD
         char buffer2[10];
         sprintf(buffer2, "%.2f", distancia);

         // Escribir el valor de la Distancia en la LCD
         write_string("D: ");
         write_string(buffer2);
         write_string(" cm");

         // Escribir el valor de la Temperatura y Frecuencia de muestreo en la terminal
         printf("Distancia: %.2f cm\n", distancia);
         printf("Frecuencia: %d Hz\n", frequency);

         // Se crea un array llamado samplesdis para almacenar varias muestras de temperatura.
         float samplesdis[num_samples];
         // Bucle for para obtener num_samples muestras de Distancia y almacenarlas en el array samples.
         for (int i = 0; i < num_samples; i++)
         {
            samplesdis[i] = read_distance();
         }
         // Se calcula el valor promedio de estas muestras utilizando la función calculate_average
         float average_value_dis = calculate_average(samplesdis, num_samples);

         char buffer3[10];
         sprintf(buffer3, "%.2f", average_value_dis);

         write_string("\nProm: ");
         write_string(buffer3);
         write_string(" cm");

         printf("Valor promedio: %.2f\n", average_value_dis, " cm");
         sleep_ms(150);
         clear_display();
      }

      // Condicional que va cambiando entre 3 frecuencias de muestre distintas
      if (button2_pressed)
      {

         if (frequency == 1)
         {
            frequency = 10;
         }

         else if (frequency == 10)
         {
            frequency = 1000;
         }

         else if (frequency == 1000)
         {
            frequency = 1;
         }

         printf("Frecuencia:  %.2f\n", frequency);

         button2_pressed = false;
      }

      // Condicional que va cambiando entre 4 distintos valores de muestreo

      if (button3_pressed)
      {

         switch (num_samples)
         {
         case 8:
            num_samples = 32;
            break;
         case 32:
            num_samples = 128;
            break;
         case 128:
            num_samples = 512;
            break;
         case 512:
            num_samples = 8;
            break;
         }

         printf("Muestras: %d\n", num_samples);

         button3_pressed = false;
      }

      period_ms = 1000 / frequency;
      sleep_ms(period_ms); // Frecuencia de muestreo
   }

   return 0;
}