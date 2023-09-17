#include <stdio.h>
#include <string.h>
#include "usuarios.h"

int verificarAcceso(const char *usuario_ingresado, const char *contrasena_ingresada) {

   char usua_str[5];
   char cont_str[7];


    for (int i = 0; i < NUM_USUARIOS; i++) {
    snprintf(usua_str, sizeof(usua_str), "%d", usuarios[i][0]);
    snprintf(cont_str, sizeof(cont_str), "%d", usuarios[i][1]);

    if (strcmp(usuario_ingresado, usua_str) == 0 &&
        strcmp(contrasena_ingresada, cont_str) == 0) {
        printf("Acceso concedido\n");
        return 0;
    }
}

printf("Acceso denegado\n");
return 1;
}