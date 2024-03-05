#include <stdio.h>
#include "pico/stdlib.h"

/**********************************************************************/
/*                             Constantes                             */
/**********************************************************************/

// Definición de pines para el control de la pantalla LCD
#define RS 0   // Se establece en BAJO cuando se envían comandos, ALTO cuando se envían datos
#define EN 1   // Se establece en BAJO para ignorar los datos entrantes, ALTO para recibirlos
#define DB0 16 // Bit de datos 0
#define DB1 17 // Bit de datos 1
#define DB2 18 // Bit de datos 2
#define DB3 19 // Bit de datos 3
#define DB4 5  // Bit de datos 4
#define DB5 4  // Bit de datos 5
#define DB6 3  // Bit de datos 6
#define DB7 2  // Bit de datos 7

// Constantes para representar valores lógicos
#define HIGH 1
#define LOW 0

// Comandos de control de la pantalla LCD
#define CLEAR_DISPLAY 0b00000001
#define RETURN_HOME 0b00000010

// Opciones para el estado de visualización, cursor y parpadeo del cursor (se deben combinar mediante operaciones OR)
#define DISPLAY_ON 0b00001100
#define DISPLAY_OFF 0b00001000
#define CURSOR_ON 0b00001010
#define CURSOR_OFF 0b00001000
#define CURSOR_BLINK_ON 0b00001001
#define CURSOR_BLINK_OFF 0b00001000

// Opciones para el modo de bits, líneas y tipo de fuente (se deben combinar mediante operaciones OR)
#define BIT_MODE_8 0b00110000
#define BIT_MODE_4 0b00100000
#define LINES_1 0b00100000
#define LINES_2 0b00101000
#define FONT_5X8 0b00100000
#define FONT_5X11 0b00100100

// Establecer los últimos 6 bits a la dirección CGRAM deseada
#define SET_CGRAM 0b01000000

// Establecer los últimos 7 bits a la dirección DDRAM deseada
#define SET_DDRAM 0b10000000

// Valores para representar los estados ON y OFF
#define ON 1
#define OFF 0

/**********************************************************************/
/*                       Variables Globales                           */
/**********************************************************************/

// se utiliza para almacenar información relacionada con el estado de visualización, cursor y parpadeo del cursor.
uint8_t d_c_cb = 0b00001000;
// se utiliza para configurar el modo de bits (4 o 8 bits), el número de líneas (1 o 2) y el tipo de fuente (5x8 o 5x11).
uint8_t b_d_f = 0b00100000;

/**********************************************************************/
/*                     Declaracion de funcioes                        */
/**********************************************************************/
uint32_t init(uint reset_set, uint enable, uint databit0, uint databit1, uint databit2, uint databit3, uint databit4, uint databit5, uint databit6, uint databit7);
// Esta función se utiliza para inicializar las conexiones con la pantalla LCD y configurar los pines de control y datos.
void pulse_enable();
// Se utiliza para generar un pulso en el pin de habilitación (EN) de la pantalla LCD. pin de habilitación durante 2us
void send_cmd(uint8_t command);
// Envia un comando a la pantalla LCD.
void send_data(uint8_t data);
// Se utiliza para enviar datos a la pantalla LCD.
void write_char(char character);
// Escribe un solo carácter en la pantalla LCD.
void write_string(char string[]);
//  Escribe una cadena de caracteres en la pantalla LCD
void clear_display();
// Limpia la LCD y restablecer la posición del cursor al inicio.
void return_home();
// Reinicia el cursor
void line_select(int line_number);
// Elije la linea en la que va a escribir
void display(uint8_t status);
// Enciende o apaga el display
void cursor(uint8_t status);
//  Encender o apagar el cursor en la pantalla LCD.
void cursor_blink(uint8_t status);
// Esta función se utiliza para encender o apagar el parpadeo del cursor en la pantalla LCD.
void bit_mode(uint8_t number_of_bits);
//  configurar el modo de bits (4 bits o 8 bits) en la pantalla LCD.
void display_lines(uint8_t number_of_lines);
// configurar el número de líneas en la pantalla LCD (1 línea o 2 líneas).
void font_type(uint8_t font_size);
// configurar el tipo de fuente en la pantalla LCD (5x8 o 5x11).
int strlength(char string[]);
// Esta función se utiliza para calcular la longitud de una cadena de caracteres (el número de caracteres en la cadena).

uint32_t init(uint reset_set, uint enable, uint databit0, uint databit1, uint databit2, uint databit3, uint databit4, uint databit5, uint databit6, uint databit7)
{

    //// Variable para almacenar una máscara de bits que representa los pines de la pantalla LCD
    uint32_t pin_mask = 0x0;
    // Arreglo que contiene los pines GPIO
    uint pins[] = {reset_set, enable, databit0, databit1, databit2, databit3, databit4, databit5, databit6, databit7};
    // Número total de pines
    uint8_t num_of_pins = 10;
    uint8_t counter;

    // Crear una máscara de bits que representa los pines de la pantalla LCD
    for (counter = 0; counter < num_of_pins; counter++)
        pin_mask |= 0x1 << pins[counter];

    // Configurar todos los pines como salidas y establecerlos en su valor por defecto (LOW)
    gpio_init_mask(pin_mask);
    gpio_set_dir_out_masked(pin_mask);
    for (counter = 0; counter < num_of_pins; counter++)
        gpio_pull_up(pins[counter]);

    // Configurar la pantalla LCD con una configuración predeterminada
    send_cmd(BIT_MODE_8 | LINES_2 | FONT_5X8);          // Configurar modo de bits, número de líneas y tipo de fuente
    send_cmd(DISPLAY_ON | CURSOR_ON | CURSOR_BLINK_ON); // Encender el display, cursor y cursor parpadeante
    clear_display();                                    // Borrar la pantalla

    return pin_mask; // Devolver la máscara de bits que representa los pines
}

void pulse_enable()
{
    // Esperar al menos 2 microsegundos para cumplir con el tiempo mínimo de habilitación
    // El tiempo mínimo de encendido-apagado de habilitación es de 1200 ns (nanosegundos),
    // pero aquí se espera 2 microsegundos para garantizar que se cumpla este tiempo.
    sleep_us(2);
    // Establecer el pin de habilitación (EN) en ALTO para activar la señal de habilitación
    gpio_put(EN, HIGH);
    // Esperar al menos 2 microsegundos con el pin de habilitación en ALTO
    sleep_us(2);
    // Establecer el pin de habilitación (EN) en BAJO para desactivar la señal de habilitación
    gpio_put(EN, LOW);
    // Esperar al menos 100 microsegundos para garantizar que se cumpla el tiempo mínimo
    // de establecimiento de comandos (minimum cmd settling time) que es de 37 microsegundos
    sleep_us(100);
    return;
}

void send_cmd(uint8_t command)
{
    // RS esta en LOW por defecto, y RW cableado a bajo nivel
    // Sólo hay que configurar los databits
    gpio_put(DB0, command & 0x1);
    gpio_put(DB1, command & 0x2);
    gpio_put(DB2, command & 0x4);
    gpio_put(DB3, command & 0x8);
    gpio_put(DB4, command & 0x10);
    gpio_put(DB5, command & 0x20);
    gpio_put(DB6, command & 0x40);
    gpio_put(DB7, command & 0x80);
    pulse_enable();
    return;
}

void send_data(uint8_t data)
{
    gpio_put(RS, HIGH); //  Aquí se establece el pin RS en ALTO para indicar que se están enviando datos a la pantalla LCD.
    send_cmd(data);     // enviar el dato proporcionado.
    gpio_put(RS, LOW);  // Después de enviar el dato, se restablece el pin RS a BAJO. Esto se hace para que los comandos futuros (si los hay) se distingan claramente de los datos.
    return;
}

// Esta función toma un carácter como entrada y llama a send_data() para enviar ese carácter a la pantalla LCD.
void write_char(char character)
{
    send_data((uint8_t)character);
    return;
}

void write_string(char string[])
{
    uint8_t counter;
    uint8_t string_length = strlength(string);

    for (counter = 0; counter < string_length; counter++)
    {
        //  Para cada carácter, verifica si es un salto de línea ('\n'). Si es un salto de línea, llama a line_select(2) para seleccionar la línea 2 en la pantalla LCD
        if (string[counter] == '\n')
            line_select(2);
        else
            // llama a write_char() para escribir el carácter en la pantalla LCD.
            write_char(string[counter]);
    }

    return;
}

void clear_display()
{
    // Se manda el comando que limpia el display
    send_cmd(CLEAR_DISPLAY);
    // espera durante 2 milisegundos antes de regresar.
    sleep_ms(2);
    return;
}

void return_home()
{
    // Envia el comando que devuelve el cursor a la posición de inicio en la pantalla LCD.
    send_cmd(RETURN_HOME);
    return;
}

// Esta función toma un número de línea como entrada (1 o 2) y selecciona la línea correspondiente en la pantalla LCD.
void line_select(int line_number)
{
    if (line_number == 1)
        // establece la dirección DDRAM para la línea 1.
        send_cmd(SET_DDRAM | 0b00000000);
    else if (line_number == 2)
        // establece la dirección DDRAM para la línea 2.
        send_cmd(SET_DDRAM | 0b01000000);
    return;
}

// Esta función controla si se debe encender o apagar la visualización en la pantalla LCD.
void display(uint8_t status)
{
    if (status == OFF)
        d_c_cb &= !0b00000100;
    else
        d_c_cb |= 0b00000100;
    send_cmd(DISPLAY_ON);
    return;
}

// Esta función controla si se debe encender o apagar la visualización del cursor en la pantalla LCD.
void cursor(uint8_t status)
{
    if (status == OFF)
        d_c_cb &= !0b00000010;
    else
        d_c_cb |= 0b00000010;
    send_cmd(d_c_cb);
    return;
}

void cursor_blink(uint8_t status)
{
    if (status == OFF)
        // se utiliza una operación de bits para apagar el bit menos significativo (0b00000001) de la variable d_c_cb.
        // Esto asegura que el bit correspondiente al parpadeo del cursor esté desactivado.
        d_c_cb &= !0b00000001;
    else
        d_c_cb |= 0b00000001;
    // se llama a la función send_cmd(d_c_cb) para enviar el comando actualizado a la pantalla LCD. Esto configura el estado del cursor según el valor de status.
    send_cmd(d_c_cb);
    return;
}

// Esta función configura el modo de bits para la pantalla LCD, que puede ser 4 bits o 8 bits.
void bit_mode(uint8_t number_of_bits)
{
    if (number_of_bits == 4)
        b_d_f &= !0b00010000;
    else if (number_of_bits == 8)
        b_d_f |= 0b00010000;
    send_cmd(b_d_f);
    return;
}
// Esta función configura el número de líneas que se mostrarán en la pantalla LCD, que puede ser 1 o 2 líneas.
void display_lines(uint8_t number_of_lines)
{
    if (number_of_lines == 1)
        b_d_f &= !0b00001000;
    else if (number_of_lines == 2)
        b_d_f |= 0b00001000;
    send_cmd(b_d_f);

    return;
}
// Esta función configura el tipo de fuente para la pantalla LCD, que puede ser 5x8 o 5x11.
void font_type(uint8_t font_size)
{
    if (font_size == FONT_5X8)
        b_d_f &= !0b00000100;
    else if (font_size == FONT_5X11)
        b_d_f |= 0b00000100;
    send_cmd(b_d_f);

    return;
}
// Esta función calcula la longitud de una cadena de caracteres string.
int strlength(char string[])
{
    // cuenta caracter por caracter
    int counter = 0;
    while (string[counter] != '\0')
        counter++;
    return counter;
}
