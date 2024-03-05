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
/**
 * @brief Funcion principal
 */