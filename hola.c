
/*

// Standard libraries
#include "luces.h"

// Include your own header files here

#define ROWS 4  // Número de filas en el teclado matricial
#define COLS 4  // Número de columnas en el teclado matricial

const uint rowPins[ROWS] = {2, 3, 4, 5}; // Cambia estos pines según tu conexión
const uint colPins[COLS] = {6, 7, 8, 9}; // Cambia estos pines según tu conexión

char getKey() {
    char keymap[ROWS][COLS] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    for (int row = 0; row < ROWS; row++) {
        // Establece la fila actual a LOW (activo bajo)
        gpio_put(rowPins[row], 1);

        for (int col = 0; col < COLS; col++) {
            if (gpio_get(colPins[col]) == 1) {
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



int main() {
	// STDIO initialization
    stdio_init_all();

    // Write your initialization code here

    // Configura pines de filas como salidas y pines de columnas como entradas
    for (int i = 0; i < ROWS; i++) {
        gpio_init(rowPins[i]);
        gpio_set_dir(rowPins[i], GPIO_OUT);
    }

    for (int i = 0; i < COLS; i++) {
        gpio_init(colPins[i]);
        gpio_set_dir(colPins[i], GPIO_IN);
        gpio_pull_down(colPins[i]);  // Habilita resistencia pull-down en pines de columna
    }
	
	
	// Infinite loop. This function shouldn't finish or return
    while (1) {
        // Write your code here
        sleep_ms(100);
        char key = getKey();

        if (key != 0) {
            printf("Tecla presionada: %c\n", key);
            sleep_ms(500); // Evita la repetición rápida de teclas
        }
    }

    
	return 0;
}

*/