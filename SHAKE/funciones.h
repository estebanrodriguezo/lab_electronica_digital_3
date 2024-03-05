/**
 * @file funciones.h
 * @version 1.0
 * @brief Funciones para el cálculo del Hash
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <time.h>
#include "hash_data.h"
// variables de keccakf
#define c 512
#define r 1088
#define b 1600

/**
 * @brief modulo Funcion para llevar números negativos a positivos
 * @param x
 * @return Módulo del número ingresado
 */

// redefinir el operador módulo
static int modulo(int x, int y)
{
	if (x < 0)
	{ // sólo cambia para valores negativos
		x = x * -1;
		return (y - (x % y)) % y; // Esto asegura que el resultado final esté dentro del rango [0, y-1] incluso cuando x es negativo.
	}
	else
	{
		return (x % y);
	}
}

/**
 * @brief Función que realiza un intercambio de bits en un byte (swap)
 * @return El byte modificado
 */
// Función que realiza un intercambio de bits en un byte de 8 bits
// Se utiliza para realizar un byte swap en un byte de 8 bits, cambiando el orden de sus bits.
static uint8_t bswap_8(uint8_t byte)
{
	return (byte & 0x80 ? 0x01 : 0) |
		   (byte & 0x40 ? 0x02 : 0) |
		   (byte & 0x20 ? 0x04 : 0) |
		   (byte & 0x10 ? 0x08 : 0) |
		   (byte & 0x08 ? 0x10 : 0) |
		   (byte & 0x04 ? 0x20 : 0) |
		   (byte & 0x02 ? 0x40 : 0) |
		   (byte & 0x01 ? 0x80 : 0);
}

/**
 * @brief Invierte el orden de los bytes en cada elemento del vector. (un vector de tipo uint8_t.)
 * @return Bytes invertidos
 */

// invierte el orden de los bytes en cada elemento del vector. (un vector de tipo uint8_t.)
static uint8_t b2lendian(uint8_t *vector, int sizev)
{
	for (int i = 0; i < sizev; i++)
	{
		vector[i] = bswap_8(vector[i]); // Esto invierte el orden de los bits en el elemento vector[i]. El resultado se almacena nuevamente en vector[i], sobrescribiendo el valor original.
	}
}
/**
 * @brief Se utiliza para realizar el relleno de datos en una cadena de bytes
 * @return Cadena de bits rellena
 */
// padding  se utiliza para realizar el relleno de datos en una cadena de bytes
static void padding(uint8_t *in_string, long long int bytes, int bloques)
{

	in_string[bytes] = (0b11111000); // Agrega 4 unos y 1 uno adicional solicitado por el estándar
	for (int i = bytes + 1; i < ((bloques + 1) * 136) - 1; i++)
	{
		in_string[i] = (0b00000000); // Completa con ceros para llenar 1079 bits
	}
	in_string[(((bloques + 1) * 136) - 1)] = 0b00000001;
} // y el último 1 para completar la cadena
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
La función padding se utiliza para realizar el relleno de datos en una cadena de bytes, generalmente como parte de un proceso de preparación de datos para algún algoritmo criptográfico o de procesamiento. En el contexto del código proporcionado, parece que se está preparando una cadena de bytes para el algoritmo Keccak (también conocido como SHA-3), que requiere un relleno específico según el estándar.

uint8_t *in_string es un puntero a una cadena de bytes que se va a rellenar.

long long int bytes es la cantidad de bytes en la cadena de bytes original antes del relleno.

int bloques representa el número de bloques que se utilizarán después del relleno.

El bucle for a continuación comienza desde bytes + 1 y continúa hasta ((bloques + 1) * 136) - 1. Esto se hace para llenar los bytes adicionales necesarios para alcanzar la longitud deseada después del relleno.

Dentro del bucle, in_string[i] = (0b00000000); establece todos los bytes adicionales en cero, lo que completa el relleno para alcanzar los 1079 bits requeridos.

Finalmente, in_string[(((bloques + 1) * 136) - 1)] = 0b00000001; agrega un "1 uno" al final de la cadena para completar el proceso de relleno según el estándar Keccak.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief
 */
// Estas constantes se utilizan para realizar operaciones específicas en cada ronda del algoritmo Keccak durante la transformación.
static const uint64_t RC[24] = {
	0x8000000000000000, 0x4101000000000000, 0x5101000000000001,
	0x0001000100000001, 0xD101000000000000, 0x8000000100000000,
	0x8101000100000001, 0x9001000000000001, 0x5100000000000000,
	0x1100000000000000, 0x9001000100000000, 0x5000000100000000,
	0xD101000100000000, 0xD100000000000001, 0x9101000000000001,
	0xC001000000000001, 0x4001000000000001, 0x0100000000000001,
	0x5001000000000000, 0x5000000100000001, 0x8101000100000001,
	0x0101000000000001, 0x8000000100000000, 0x1001000100000001};
// Cada elemento en el array RC es una constante hexadecimal de 64 bits que se utiliza en las rondas

/**
 * @brief Funcion denominada Theta
 * @param state
 * @return Estado actualizado después de aplicar Theta
 */
static void theta(uint64_t *state)
{
	// Definicion de C y D (arrays)
	uint64_t C[5] = {0};
	uint64_t D[5] = {0};

	// Calculamos el valor de C
	for (int i = 0; i < 5; i++) // El propósito de esta operación es introducir no linealidad en los cálculos y aumentar la seguridad del algoritmo.
	{
		C[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
	}

	// Calculamos el valor de D
	for (int i = 0; i < 5; i++)
	{
		int prev = modulo((i - 1), 5);
		int next = modulo((i + 1), 5);

		D[i] = C[prev] ^ ((C[next] << 63) | (C[next] >> 1)); // una operación de desplazamiento (shift) a la izquierda de 63 bits en C[(i+1) % 5] (el elemento siguiente en C) y una operación de rotación (shift circular) a la derecha de 1 bit en C[(i+1) % 5].
															 // intercambios de theta
	}

	for (int i = 0; i < 5; i++) // Esto se hace para cada palabra en state y es lo que se conoce como "Theta swaps".
	{
		for (int j = 0; j < 5; j++)
		{
			state[i + 5 * j] = state[i + 5 * j] ^ D[i]; // xor de theta
		}
	}
}

/**
 * @brief Funcion denominada Rho
 * @param state
 * @return Estado actualizado después de aplicar Rho
 */

static void rho(uint64_t *state)
{
	int jbuffer;
	int i, j;
	i = 1;
	j = 0;
	for (int t = 0; t < 24; t++) // Se repite 24 veces  debido a que esto está relacionado con las rondas de Keccak.
	{
		uint64_t temp = state[i + 5 * j];																	   // Almacenamos temporalmente el valor de state[i + 5 * j] en "temp".
		state[i + 5 * j] = ((((temp) << (64 - (t + 1) * (t + 2) / 2))) | ((temp) >> ((t + 1) * (t + 2) / 2))); // Se realiza una operación de desplazamiento a la izquierda en un elemento específico del estado state[i+5*j].
		// Esta fórmula desplaza los bits hacia la izquierda y hacia la derecha en state[i+5*j] y luego realiza una operación OR bit a bit entre los resultados de los desplazamientos.
		jbuffer = j;
		j = modulo((2 * i + 3 * j), 5);
		i = jbuffer;
		// Esto es parte de la lógica de la transformación Rho para determinar cuál es el próximo elemento en el estado que se modificará en la siguiente iteración del bucle.
	}
}

/**
 * @brief Funcion denominada Pi
 * @param state
 * @return Estado actualizado después de aplicar Pi
 */
// Se encarga de reorganizar los valores en el estado interno del algoritmo siguiendo un patrón específico.
static void pi(uint64_t *state)
{
	int i, j;
	uint64_t s_copy[25] = {0}; // Se necesita copia del estado para poder operar

	for (i = 0; i < 25; i++)
	{
		s_copy[i] = state[i]; // Copiamos el estado original a la copia temporal
	}
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			// Reorganizamos el estado original basado en la copia temporal
			state[i + 5 * j] = s_copy[modulo((i + 3 * j), 5) + 5 * i /*calcula la nueva posición a partir de la fórmula de reorganización específica.*/];
		}
	}
}

/**
 * @brief Funcion denominada Xci-Iota
 * @param state
 * @return Estado actualizado después de aplicar Xci-Iota
 */
// Esta transformación consta de tres pasos: XOR (X), Chi (C), e Iota (I)
static void xci_iota(uint64_t *state, int ir)
{
	uint64_t s_copy[25] = {0}; // Creamos una copia temporal del estado
	int i, j;
	for (i = 0; i < 25; i++)
	{
		s_copy[i] = state[i]; // Copiamos el estado original a la copia temporal
	}

	// Paso XOR (X) y Chi
	for (i = 0; i < 5; i++)
	{ // Bucle externo para recorrer las filas del estado.
		for (j = 0; j < 5; j++)
		{											  // Bucle interno para recorrer las columnas del estado.
			uint64_t A = s_copy[i + 5 * j];			  // Obtenemos el valor de A del estado copiado.
			uint64_t B = s_copy[(i + 1) % 5 + 5 * j]; // Obtenemos el valor de B del estado copiado.
													  // se realiza una operación XOR entre el elemento A, el elemento B y una máscara de bits 0xFFFFFFFFFFFFFFFFULL. Esto cambia algunos bits en el elemento B según el valor de C.
			uint64_t C = s_copy[(i + 2) % 5 + 5 * j]; // Obtenemos el valor de C del estado copiado.

			state[i + 5 * j] = A ^ ((B ^ 0xFFFFFFFFFFFFFFFFULL) & C); // Paso de Chi los bits de A se modifican según los valores de B y C.
			// Realizamos la operación XOR (X) y Chi en el elemento actual del estado original.
		}
	}

	// Paso Iota (I)
	state[0] ^= RC[ir]; // Realizamos la operación Iota (I) en el primer elemento del estado original utilizando la constante RC correspondiente.
}

/**
 * @brief Aplica las transformaciones Theta, Rho, Pi y Xci Iota
 * @param state
 * @return Estado actualizado después de aplicar todas las transformaciones
 */
static void RND(uint64_t *state, int ir)
{
	theta(state);		 // Llama a la función theta
	rho(state);			 // Llama a la función rho
	pi(state);			 // Llama a la función pi
	xci_iota(state, ir); // Llama a la función xci_iota
} // Estas transformaciones se aplican repetidamente en cada ronda del algoritmo para procesar los datos y calcular el hash final.

/**
 * @brief Ejecuta la funcion de esponja
 * @return Valor del hash en bits
 */
// sponge
static void sponge_keccakf(uint8_t *in_string, uint8_t s_uint8[], uint64_t s_uint64[], uint64_t s[], int bloques)
{
	int ir;
	for (int j = 0; j < (bloques + 1); j++)
	{
		for (int i = 0; i < (200); i++)
		{
			if (i < 136)
			{
				s_uint8[i] = in_string[i + (136 * j)]; // Copia los primeros 1088 bits del mensaje en s_uint8
			}
			else
				s_uint8[i] = 0b00000000; // Rellena con ceros desde el bit 1088 (byte # 136) hasta el 1599 (byte # 199)
		}
		// Convierte los 200 bytes (1600 bits) en bloques de 64 bits
		for (int k = 0; k < 25; k++)
		{
			for (int l = 0; l < 8; l++)
			{
				s_uint64[k] = (s_uint64[k] << 8) | (uint64_t)(s_uint8[l + (8 * k)]); // Cada entrada se desplaza 8 bits y se añaden 8 bits, al final deberías tener 64 bits
			}
			s[k] ^= s_uint64[k]; // Realiza una operación XOR entre los bloques de 64 bits y el estado s
		}
		// Aplica las 24 rondas de transformaciones Keccak
		for (ir = 0; ir < 24; ir++)
		{				// Itera a través de cada transformación
			RND(s, ir); // Llama a la función RND que aplica las transformaciones
		}
	}
}

/**
 * @brief Se encarga de imprimir el hash calculado en formato hexadecimal.
 * @return Valor del hash en hexadecimal
 */
// Se encarga de imprimir el hash calculado en formato hexadecimal.
static void keyhex(uint64_t s[], int d, int slen)
{
	int count = 56;
	int i = 0;
	for (int j = 0; j < d / 8; j++)
	{
		printf("%02x", bswap_8(s[i] >> count)); // Aplica la función de swap a cada byte y lo imprime en hexadecimal
		count -= 8;								// Reduce el contador para cambiar al siguiente bloque de 8 bits
		if (count < 0)
		{
			count = 56;
			i++; // Restablece el contador para el siguiente bloque
			if (i > slen)
			{
				break;
			}
		}
	}
}
