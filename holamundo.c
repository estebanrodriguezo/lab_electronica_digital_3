#include "pico/stdlib.h"
#include "acceso.h"
#include "hardware/gpio.h"

#define ROWS 4 // Número de filas en el teclado
#define COLS 4 // Número de columnas en el teclado

const uint rowPins[ROWS] = {2, 3, 4, 5};
const uint colPins[COLS] = {6, 7, 8, 9};

char getKey()
{
  char keymap[ROWS][COLS] = {
      {'1', '2', '3', 'A'},
      {'4', '5', '6', 'B'},
      {'7', '8', '9', 'C'},
      {'*', '0', '#', 'D'}};

  for (int row = 0; row < ROWS; row++)
  {
    // Establece la fila actual a LOW (activo bajo)
    gpio_put(rowPins[row], 1);

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
  stdio_init_all();
  // usuario_ingresado[usuario_index] = '\0';
  // contrasena_ingresada[contrasena_index] = '\0';

  // Configura pines de filas como salidas y pines de columnas como entradas
  for (int i = 0; i < ROWS; i++)
  {
    gpio_init(rowPins[i]);
    gpio_set_dir(rowPins[i], GPIO_OUT);
  }

  for (int i = 0; i < COLS; i++)
  {
    gpio_init(colPins[i]);
    gpio_set_dir(colPins[i], GPIO_IN);
    gpio_pull_down(colPins[i]); // Habilita resistencia pull-down en pines de columna
  }


  char usuario_ingresado[] = {'_', '_', '_', '\0'} ;
  char contrasena_ingresada[3];
  printf("array: %s\n", usuario_ingresado);
  int usuario_index = 0;
  int contrasena_index = 0;
  printf("contrasena indexada: %d\n", contrasena_index);
  printf("Usuario index: %d\n", usuario_index);


  sleep_ms(2000);

  printf("Ingrese su usuario: \n");
  int i = 0;
  while (1)
  {
    sleep_ms(100);
    char key = getKey();
    if (key != 0)
    {
      printf("array: %s\n", usuario_ingresado);
      printf("contrasena indexada: %d\n", contrasena_index);
      printf("Tecla presionada: %c\n", key);
      sleep_ms(500); // Evita la repetición rápida de teclas
      i += 1;
      printf("contador %d\n", i);
      usuario_ingresado[(i-1)%3] = key;


      printf("array: %s\n", usuario_ingresado);
    }




    // Write your code here
    // encenderledamarillo();

    /*if (key == '*') {
          usuario_ingresado[usuario_index] = '\0';
          printf("usuario: %c\n", usuario_ingresado,'\n');
          printf("Ingrese la clave de usuario: %c\n");

      while (1){
      char key = getKey();
      //apagarledamarillo();

      if (key == '*') {
          contrasena_ingresada[contrasena_index] = '\0';
          printf("contraseña: %c\n", contrasena_ingresada);
      //apagarledamarillo();
      break; // Salir del bucle cuando se presiona asterisco ('*')
      }

      else if (contrasena_index < 6) { // Evitar desbordamiento
          printf("tecla: %c\n", key);
          contrasena_ingresada[contrasena_index++] = key; // Agregar la tecla a la cadena y aumentar el índice
      }
     }
  }

    else if (usuario_index < 4) { // Evitar desbordamiento
      printf("tecla %c\n", key);
      usuario_ingresado[usuario_index++] = key; // Agregar la tecla a la cadena y aumentar el índice
    }  */
  }

  int resultado = verificarAcceso(usuario_ingresado, contrasena_ingresada);
  return 0;
}

/*
    while (1) {
        //parpadearledamarillo();
        char key = getKey();

      if (key == '*') {
        contrasena_ingresada[contrasena_index] = '\0';
        printf("contraseña: ", contrasena_ingresada);
        //apagarledamarillo();
        break; // Salir del bucle cuando se presiona asterisco ('*')
    }
     else if (contrasena_index < 6) { // Evitar desbordamiento
        printf("tecla: ", key);
        contrasena_ingresada[contrasena_index++] = key; // Agregar la tecla a la cadena y aumentar el índice
      }

    }
    */
