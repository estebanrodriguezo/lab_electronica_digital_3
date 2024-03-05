#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"

/////////////////////////////////////EEPROM/////////////////////////////////////

#define EEPROM_ADDR 0b1010000 // Dirección I2C de la EEPROM (A0-A2 a tierra)

// Estructura para almacenar posicion
typedef struct
{
    float latitude;
    float longitude;
} GPSData;

void i2c_write_gps_data(i2c_inst_t *i2c, uint8_t address, GPSData data, uint16_t offset)
{
    uint8_t buffer[sizeof(GPSData) + 2];
    memcpy(buffer, &offset, 2);
    memcpy(buffer + 2, &data, sizeof(GPSData));
    i2c_write_blocking(i2c, EEPROM_ADDR, buffer, sizeof(GPSData) + 2, false);
    sleep_ms(10);
}

GPSData i2c_read_gps_data(i2c_inst_t *i2c, uint8_t address, uint16_t offset)
{
    GPSData data;
    uint8_t buffer[sizeof(GPSData)];
    i2c_write_blocking(i2c, EEPROM_ADDR, (uint8_t *)&offset, 2, true);
    i2c_read_blocking(i2c, EEPROM_ADDR, buffer, sizeof(GPSData), false);
    memcpy(&data, buffer, sizeof(GPSData));
    return data;
}

void i2c_read_last_n_structs(i2c_inst_t *i2c, uint8_t address, int indice, uint16_t n) {
    
    uint16_t start_address = indice- n;
    printf("Direccion de inicio %d \n", start_address);
    printf("Indice %d \n",indice);
    GPSData data;
    uint8_t buffer[sizeof(GPSData)];
    int indicelectura=indice;

    printf("Últimas %d estructuras leídas desde la EEPROM:\n", n);

    // Imprimir el recorrido en formato KML al final del programa
    printf("\nImprimir el recorrido en formato KML al final del programa...\n");

    printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    printf("<kml xmlns=\"http://www.opengis.net/kml/2.2\"> <Document>\n");
    printf(" <name>Rutas</name>\n");
    printf(" <description>Ejemplos de rutas. Observa que la etiqueta tessellate se establece de forma predeterminada\n");
    printf(" en 0. Si quieres crear líneas teseladas, deben crearse\n");
    printf(" (o editarse) directamente en KML.</description>\n");

    printf(" <Style id=\"yellowLineGreenPoly\">\n");
    printf(" <LineStyle>\n");
    printf(" <color>7f00ffff</color>\n");
    printf(" <width>4</width>\n");
    printf(" </LineStyle>\n");
    printf(" <PolyStyle>\n");
    printf(" <color>7f00ff00</color>\n");
    printf(" </PolyStyle>\n");
    printf(" </Style>\n");

    printf(" <Placemark>\n");
    printf(" <name>Recorrido GPS</name>\n");
    printf(" <description>Recorrido GPS generado</description>\n");
    printf(" <styleUrl>#yellowLineGreenPoly</styleUrl>\n");
    printf(" <LineString>\n");
    printf(" <extrude>1</extrude>\n");
    printf(" <tessellate>1</tessellate>\n");
    printf(" <altitudeMode>absolute</altitudeMode>\n");
    printf(" <coordinates>\n");

    /*for (int i = 0; i < num_entries; i++)
    {
        printf("%f, %f\n", datos[i].latitude, datos[i].longitude);
    }*/

    for (uint16_t i = start_address; i < indicelectura; i++){

    GPSData datosLeidos = i2c_read_gps_data(i2c, EEPROM_ADDR,i);
    printf("%f, %f\n", datosLeidos.longitude, datosLeidos.latitude);
    }

    printf(" </coordinates>\n");
    printf(" </LineString>\n");
    printf(" </Placemark>\n");
    printf(" </Document>\n");
    printf(" </kml>\n");

    printf("Archivo KML generado con éxito: route.kml\n");
}

/////////////////////////////////////EEPROM/////////////////////////////////////

// Define uart properties for the GPS
const uint32_t GPS_BAUDRATE = 9600;
const uint8_t GPS_TX = 4, GPS_RX = 5;
const uint8_t DATABITS = 8;
const uint8_t STARTBITS = 1;
const uint8_t STOPBITS = 1;

// Calculate the delay between bytes
const uint8_t BITS_PER_BYTE = STARTBITS + DATABITS + STOPBITS;
const uint32_t MICROSECONDS_PER_SECOND = 1000000;
const uint32_t GPS_DELAY = BITS_PER_BYTE * MICROSECONDS_PER_SECOND / GPS_BAUDRATE;

// Definir el pin del LED
const uint8_t LED_PIN = 25; // Cambia esto al número de pin que estás utilizando para el LED

size_t uart_read_line(uart_inst_t *uart, char *buffer, const size_t max_length)
{
    size_t i;
    // Receive the bytes with as much delay as possible without missing data
    buffer[0] = uart_getc(uart);
    for (i = 1; i < max_length - 1 && buffer[i - 1] != '\n'; i++)
    {
        sleep_us(GPS_DELAY);
        buffer[i] = uart_getc(uart);
    }

    // End the string with a terminating 0 and return the length
    buffer[i] = '\0';
    return i;
}

bool is_correct(const char *message, const size_t length)
{
    if (strncmp(message, "$GNRMC", 6) != 0)
    {
        return false;
    }
    char sum = 0;
    char checksum[3];
    size_t i;

    // The message should start with $ and end with \r\n
    if (message[0] != '$' || message[length - 1] != '\n' || message[length - 2] != '\r')
    {
        return false;
    }

    // Calculate the checksum
    for (i = 1; i < length && message[i] != '*'; i++)
    {
        sum ^= message[i];
    }

    // If the current character isn't *, the message doesn't contain it and is invalid
    if (message[i] != '*')
    {
        return false;
    }

    // Convert the checksum to a hexadecimal string
    for (size_t i = 0; i < 2; i++)
    {
        if (sum % 16 < 10)
        {
            checksum[1 - i] = '0' + sum % 16;
        }
        else
        {
            checksum[1 - i] = 'A' + sum % 16 - 10;
        }
        sum >>= 4;
    }
    checksum[2] = '\0';

    // Return whether the checksum is equal to the found checksum
    return strncmp(checksum, message + i + 1, 2) == 0;
}

void send_with_checksum(uart_inst_t *uart, const char *message, const size_t length)
{
    char sum = 0;
    char checksum[3];

    // Calcute the checksum
    for (size_t i = 0; i < length && message[i] != '*'; i++)
    {
        sum ^= message[i];
    }

    // Convert the checksum to a hexadecimal string
    for (size_t i = 0; i < 2; i++)
    {
        if (sum % 16 < 10)
        {
            checksum[1 - i] = '0' + sum % 16;
        }
        else
        {
            checksum[1 - i] = 'A' + sum % 16 - 10;
        }
        sum >>= 4;
    }
    checksum[2] = '\0';

    // Send the message to the GPS in the expected format
    uart_putc_raw(uart, '$');
    uart_puts(uart, message);
    uart_putc(uart, '*');
    uart_puts(uart, checksum);
    uart_puts(uart, "\r\n");
}

// Función para extraer longitud y latitud de un mensaje GPRMC
void extract_lat_lon(const char *message, float *latitude, float *longitude, char *lat_direction, char *lon_direction)
{
    // Asumiendo que el formato es "$GPRMC,time,status,lat,N/S,lon,E/W,speed,course,date,mag,var,E/W*checksum\r\n"
    char *token = strtok((char *)message, ",");
    int count = 0;
    while (token != NULL)
    {
        if (count == 3)
        {
            // Latitud
            *latitude = atof(token);
        }
        else if (count == 4)
        {
            // Dirección de la latitud (N o S)
            *lat_direction = token[0];
        }
        else if (count == 5)
        {
            // Longitud
            *longitude = atof(token);
        }
        else if (count == 6)
        {
            // Dirección de la longitud (E o W)
            *lon_direction = token[0];
        }
        token = strtok(NULL, ",");
        count++;
    }
}

#define MAX_POSITIONS 60

// Function to read a line from UART with a timeout
size_t uart_read_line_timeout(uart_inst_t *uart, char *buffer, const size_t max_length, uint32_t timeout_us)
{
    size_t i;
    for (i = 0; i < max_length - 1; i++)
    {
        uint64_t start_time = time_us_64();
        while (!uart_is_readable(uart))
        {
            if (time_us_64() - start_time > timeout_us)
            {
                buffer[i] = '\0';
                return i;
            }
        }
        buffer[i] = uart_getc(uart);
        if (buffer[i] == '\n')
        {
            break;
        }
    }
    buffer[i + 1] = '\0';
    return i + 1;
}

// Fuera de la función main
char gps_status = 'U'; // 'U' representa "Desconocido" inicialmente
uint64_t gps_link_time = 0;
uint64_t current_time = 0;

uint16_t total_bytes = 32768; // Memoria total de nuestra EEPROM
int fetch_number = 0;
int positions_count = 1;

void core1_entry()
{ ////////////////////uso de otro core/////////////////////
    bool isCommandMode = false;

    while (1)
    {

        i2c_inst_t *i2c = i2c0;
        gpio_set_function(20, GPIO_FUNC_I2C);
        gpio_set_function(21, GPIO_FUNC_I2C);
        gpio_pull_up(20);
        gpio_pull_up(21);

        i2c_init(i2c, 100 * 1000); // Inicializamos la comunicación I2C a 100 kHz

        char command[15];
        scanf("%s", command);

        if (strcmp(command, "START") == 0)
        {
            printf("Terminal de comandos: \n");
            isCommandMode = true;
        }
        else if (strcmp(command, "STOP") == 0)
        {
            printf("Comandos detenidos.\n");
            isCommandMode = false;
        }
        else if (strcmp(command, "STATUS") == 0)
        {

            if (isCommandMode)
            {
                // Comando para obtener el status del GPS
                printf("Estado del GPS: %c\n", gps_status);
            }
            else
            {
                printf("Comando no reconocido: %s\n", command);
            }
        }
        else if (strncmp(command, "FETCH", 5) == 0)
        {
            sscanf(command, "FETCH%d", &fetch_number);
            i2c_read_last_n_structs(i2c, EEPROM_ADDR, positions_count, fetch_number);
            sleep_ms(1000);
        }
        else if (strcmp(command, "LINKTIME") == 0 && isCommandMode)
        {

            // Comando para obtener el tiempo de enlace del GPS
            uint64_t current_time = time_us_64();
            // uint64_t link_time_seconds = (current_time - gps_link_time) / 1000000; // Convertir a segundos

            double gps_link_time_s = (double)gps_link_time / 1000000;
            printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa: %.6f\n", gps_link_time_s);
            double current_time_s = (double)current_time / 1000000;
            printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb: %.6f\n", current_time_s);

            double link_time_seconds = (current_time_s - gps_link_time_s);
            // Print the timestamp in seconds
            printf("Timestamp in seconds: %.6f\n", link_time_seconds);
        }
        else if (strcmp(command, "CLEAN") == 0 && isCommandMode)
        {
            printf("Limpiando memoria \n");

            for (int i = 0; i < positions_count; i++)
            {
                GPSData misDatosnulos;
                misDatosnulos.latitude = 0.0;
                misDatosnulos.longitude = 0.0;
                i2c_write_gps_data(i2c, EEPROM_ADDR, misDatosnulos, i);
            }
        }

        else if (isCommandMode)
        {
            // Process other commands here
            if (strcmp(command, "EXAMPLE_COMMAND") == 0)
            {
                printf("Executing EXAMPLE_COMMAND\n");
                // Add the actions for the EXAMPLE_COMMAND
            }
            else
            {
                printf("Comando no reconocido: %s\n", command);
            }
        }
    }
}

int main()
{

    // Configurar el pin del LED como salida
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    char message[256];
    const char CONFIGURATIONS[] = "PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";

    uart_inst_t *terminal_uart = uart0; // Usaremos UART 0 para la terminal
    uart_inst_t *gps_uart = uart1;      // Usaremos UART 1 para el GPS

    // Initialize the standard IO and the uart for the gps
    stdio_init_all();
    // Inicializar y ejecutar el segundo núcleo
    multicore_launch_core1(core1_entry);

    // INICIALIZAR I2C

    i2c_inst_t *i2c = i2c0;
    gpio_set_function(20, GPIO_FUNC_I2C);
    gpio_set_function(21, GPIO_FUNC_I2C);
    gpio_pull_up(20);
    gpio_pull_up(21);

    i2c_init(i2c, 100 * 1000); // Inicializamos la comunicación I2C a 100 kHz

    uart_init(gps_uart, GPS_BAUDRATE);
    uart_init(terminal_uart, GPS_BAUDRATE);

    // Don't convert \n to \r\n
    uart_set_translate_crlf(gps_uart, false);

    // Enable the uart functionality for the pins connected to the GPS
    gpio_set_function(GPS_TX, GPIO_FUNC_UART);
    gpio_set_function(GPS_RX, GPIO_FUNC_UART);

    // Disable all types except GPRMC
    send_with_checksum(gps_uart, CONFIGURATIONS, sizeof(CONFIGURATIONS));

    // Position positions[MAX_POSITIONS];

    while (1)

    {

        // Read a line from the GPS data
        const size_t length = uart_read_line(gps_uart, message, sizeof(message));

        // Skip to the next iteration, if the data is not correct or not of the correct type
        if (!is_correct(message, length))
        {
            continue;
        }

        // Print the received line of data
        printf("%s", message);

        if (is_correct(message, length))
        {
            // Verificar si es un mensaje GPRMC

            if (strstr(message, "$GNRMC,") != NULL)
            {
                float latitude, longitude;
                char lat_direction, lon_direction;
                char status;
                extract_lat_lon(message, &latitude, &longitude, &lat_direction, &lon_direction);
                latitude /= 100;
                longitude /= 100;
                int grados = (int)latitude;
                float minutos = latitude - grados;
                minutos = minutos / 60.0;
                minutos = minutos * 100;
                latitude = grados + minutos;

                int grados2 = (int)longitude;
                float minutos2 = longitude - grados2;
                minutos2 = minutos2 / 60.0;
                minutos2 = minutos2 * 100;
                longitude = (grados2 + minutos2) * (-1);

                // Obtener el estado de conexión del GPS
                status = message[18];

                // Actualizar el estado del GPS
                gps_status = status;

                // Encender o apagar el LED según el estado de conexión
                if (status == 'A')
                {
                    gpio_put(LED_PIN, true); // Encender el LED
                                             // Actualizar el estado del GPS y el tiempo de enlace
                    gps_status = status;
                    gps_link_time = time_us_64();

                    // Guardar la posición en el arreglo
                    if (positions_count < MAX_POSITIONS)
                    {

                        GPSData gpsData;
                        gpsData.latitude = latitude;
                        gpsData.longitude = longitude;
                        i2c_write_gps_data(i2c, EEPROM_ADDR, gpsData, positions_count);
                        positions_count++;
                    }
                    else {
                        positions_count = 0;
                    }

                }
                else
                {
                    uint64_t gps_link_time = 0;

                    gpio_put(LED_PIN, false); // Apagar el LED
                    //printf("Latitude: %f %c, Longitude: %f %c, Status: %c\n", latitude, lat_direction, longitude, lon_direction, status);
                }
            }
        }
    }

    return 0;
}