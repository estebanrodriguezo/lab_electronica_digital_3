//--------------------------------------------------------------------------------------
//------- CONTROL DE ACCESO ------------------------------------------------------------
//------- Uso de Raspberry Pi Pico (GPIO) ----------------------------------------------
//------- Por: Esteban Rodriguez Ordoñez - esteban.rodriguezo@udea.edu.co --------------
//------- Maria Salome Garces Montero - msalome.garces@udea.edu.co ---------------------
//------- Facultad de Ingeniería - Departamento de Electrónica y Telecomunicaciones ----
//------- Electronica Digital III ------------------------------------------------------
//------- Septiembre de 2023 -----------------------------------------------------------
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//-- 1. Importar bibliotecas -----------------------------------------------------------
//--------------------------------------------------------------------------------------

#include "pico/stdlib.h"
#include "acceso.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "luces.h"

#define ROWS 4 // Número de filas en el teclado
#define COLS 4 // Número de columnas en el teclado

// Se configuran dos arreglos (rowPins y colPins) para especificar los pines GPIO que se utilizarán para conectar y leer un teclado matricial
const uint rowPins[ROWS] = {2, 3, 4, 5};
const uint colPins[COLS] = {6, 7, 8, 9};

// Función que se utiliza para obtener una tecla presionada
char getKey()
{

  // Aquí se declara una matriz bidimensional llamada keymap que contiene los caracteres correspondientes a cada tecla en el teclado.
  char keymap[ROWS][COLS] = {
      {'1', '2', '3', 'A'},
      {'4', '5', '6', 'B'},
      {'7', '8', '9', 'C'},
      {'*', '0', '#', 'D'}};

  // Se inicia un bucle for para iterar a través de las filas en la matriz del teclado.
  for (int row = 0; row < ROWS; row++)
  {
    // Establece la fila actual a LOW (activo bajo)
    gpio_put(rowPins[row], 1);

    // Bucle anidado que iterar a través de las columnas en la matriz del teclado.
    for (int col = 0; col < COLS; col++)
    {
      if (gpio_get(colPins[col]) == 1)
      {
        // La columna actual está activada (LOW)
        // Devuelve el carácter correspondiente en el mapa
        gpio_put(rowPins[row], 0); // Restaura la fila a HIGH
        return keymap[row][col];
      }
    }

    // Restaura la fila a HIGH
    gpio_put(rowPins[row], 0);
  }

  return 0; // No se presionó ninguna tecla
}

int main()
{
  // Función que prepara el sistema para el uso de las funciones de entrada/salida estándar en la Raspberry Pi Pico.
  stdio_init_all();

  // Configura pines de filas como salidas y pines de columnas como entradas
  for (int i = 0; i < ROWS; i++)
  {
    gpio_init(rowPins[i]);              // Inicializa el pin GPIO correspondiente a la fila con la función
    gpio_set_dir(rowPins[i], GPIO_OUT); // Establece la dirección del pin como salida utilizando gpio_set_dir().
  }

  for (int i = 0; i < COLS; i++)
  {
    gpio_init(colPins[i]);             // Inicializa el pin GPIO correspondiente a la columna
    gpio_set_dir(colPins[i], GPIO_IN); // Establece la dirección del pin como entrada utilizando gpio_set_dir().
    gpio_pull_down(colPins[i]);        // Habilita resistencia pull-down en pines de columna
  }

  // Estas líneas declara dos matrices de caracteres llamada usuario_ingresado y contrasena_ingresada.
  // Inicializa las matrices con 5 y 7  caracteres subrayados (_) seguidos de un carácter nulo ('\0').
  char usuario_ingresado[] = {'_', '_', '_', '_', '\0'};
  char contrasena_ingresada[] = {'_', '_', '_', '_', '_', '_', '\0'};
  // Estas variables se utilizarán como índices para rastrear la posición actual dentro de las matrices usuario_ingresado y contrasena_ingresada.
  int usuario_index = 0;
  int contrasena_index = 0;

  // Se realiza un sleep de 2s, para poder observar en la terminal el "Ingresar usuario"
  sleep_ms(2000);
  printf("Ingrese su usuario: \n");
  // Se inicializa variable que se usara como un contador.
  int i = 0;

  // Se llama la funcion que enciende el led amarillo mientras se ingresa el susuario.
  encenderledamarillo();
  while (1)
  {

    // En cada iteración del bucle, se llama a la función getKey() para obtener una tecla presionada y se almacena en la variable key
    char key = getKey();

    // Condicional que asegura que se estan precionando teclas
    if (key != 0)
    {
      i += 1;
      if (i == 5)
      {
        printf("Ingrese su contraseña: \n");
      }

      // Si i es menor que 5, significa que el usuario está ingresando su nombre de usuario.
      if (i < 5)
      {
        sleep_ms(500); // Evita la repetición rápida de teclas
        usuario_ingresado[(i - 1) % 4] = key;
        printf("array: %s\n", usuario_ingresado);
      }
      // Si i está en el rango de 4 a 9, esto significa que el usuario está ingresando su contraseña.
      else if (4 <= i < 10)
      {
        // En este caso, se apaga un LED amarillo (apagarledamarillo()), para dar a entender que se ingreso el usuario
        apagarledamarillo();
        printf("Tecla presionada: %c\n", key);
        sleep_ms(500); // Evita la repetición rápida de teclas

        contrasena_ingresada[(i + 1) % 6] = key;
        printf("contraseña ingresada: %s\n", contrasena_ingresada);
      }
      // Si i es igual o mayor que 10, significa que se han ingresado tanto el nombre de usuario como la contraseña.
      if (i >= 10)
      {
        // Al cumplirse la condicion, se utiliza break para salir del bucle while.
        break;
      }
    }
  }

  // Después de salir del bucle, se llama a la función verificarAcceso() para verificar el acceso con el nombre de usuario y la contraseña ingresados.
  int resultado = verificarAcceso(usuario_ingresado, contrasena_ingresada);
  if (resultado == 0)
  {
    encenderledverde();
  }
  else if (resultado == 1)
  {
    encenderledrojo();
  }
  return 0;
}
