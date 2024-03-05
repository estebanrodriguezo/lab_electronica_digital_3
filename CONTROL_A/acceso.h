/**
 * @file acceso.h
 * @version 1.0
 * @brief Funciones para el acceso de usuarios
 */

#include <stdio.h>
#include <string.h>
#include "usuarios.h"

// Se declara una función llamada verificar Acceso. Esta función toma dos argumentos: usuario_ingresado y contrasena_ingresada,
// que apuntan a cadenas  constantes (const char *).
// La función verifica si las credenciales proporcionadas coinciden con la información almacenada en la base de datos del usuario.

/**
 * @brief funcion que verifica los usuarios
 * @param x
 * @return verificar Acceso
 */

int verificarAcceso(const char *usuario_ingresado, const char *contrasena_ingresada)
{

    // Estos arreglos se utilizarán para convertir los números enteros almacenados en la base de datos de usuarios en cadenas de caracteres para su comparación.
    char usua_str[5];
    char cont_str[7];

    // bucle for que itera a través de cada usuario en la base de datos. El número total de usuarios está definido en alguna parte del código como NUM_USUARIOS
    for (int i = 0; i < NUM_USUARIOS; i++)
    {

        // snprintf se utiliza para formatear y almacenar los valores de usuario y contraseña del usuario actual en las cadenas usua_str y cont_str.
        // Estos valores se convierten de enteros (usuarios[i][0] y usuarios[i][1]) a cadenas de caracteres para facilitar su comparación con las credenciales ingresadas.
        snprintf(usua_str, sizeof(usua_str), "%d", usuarios[i][0]);
        snprintf(cont_str, sizeof(cont_str), "%d", usuarios[i][1]);

        // Se compara la cadena usuario_ingresado con usua_str y la cadena contrasena_ingresada con cont_str
        // Si se cumple esta condicion, se da el acceso al usuario, de lo contrario no se hara.
        if (strcmp(usuario_ingresado, usua_str) == 0 &&
            strcmp(contrasena_ingresada, cont_str) == 0)
        {
            printf("Acceso concedido\n");
            return 0;
        }
    }

    printf("Acceso denegado\n");
    return 1;
}