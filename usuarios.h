#include <stdint.h>
#include <stdio.h>

#define filas
#define columnas

// Se declara una matriz bidimensional llamada usuarios de tipo int.
// Matriz que tiene 10 filas y 2 columnas. La primera columna contiene el usuario y la segunda su respectiva contraseña.

int usuarios[10][2] = {
    {1234, 123456},
    {4567, 678901},
    {7890, 412345},
    {3123, 123456},
    {3124, 123456},
    {3125, 123456},
    {3126, 123456},
    {3127, 123456},
    {2128, 123456},
    {2129, 123456}};

//Se calcula el número de usuarios almacenados en la matriz usuarios
//Dividir el tamaño total por el tamaño de un usuario proporciona el número de usuarios en la matriz.
int NUM_USUARIOS = (sizeof(usuarios) / sizeof(usuarios[0]));