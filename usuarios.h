#include <stdint.h>
#include <stdio.h>

#define filas
#define columnas

int usuarios[10][2]= {
    {1234,123456},
    {4567,678901},
    {7890,412345},
    {3123,123456},
    {3124,123456},
    {3125,123456},
    {3126,123456},
    {3127,123456},
    {2128,123456},
    {2129,123456}
    };
    

int NUM_USUARIOS= (sizeof(usuarios) / sizeof(usuarios[0]));