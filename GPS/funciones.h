#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
#include "hardware/gpio.h"

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
void extract_lat_lon(const char *message, double *latitude, double *longitude)
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
        else if (count == 5)
        {
            // Longitud
            *longitude = atof(token);
        }
        token = strtok(NULL, ",");
        count++;
    }
}

void create_kml_file(double latitude, double longitude, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    // Escribir el encabezado del archivo KML
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
    fprintf(file, "<Placemark>\n");
    fprintf(file, "<Point>\n");
    fprintf(file, "<coordinates>%f,%f</coordinates>\n", longitude, latitude);
    fprintf(file, "</Point>\n");
    fprintf(file, "</Placemark>\n");
    fprintf(file, "</kml>\n");

    fclose(file);
}
