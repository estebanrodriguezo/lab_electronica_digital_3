/**
 * @file SHAKE256.c
 * @version 1.0
 * @brief Archivo principal en el que se leen los elementos y se aplican las funciones definidas en funciones.h
*/

#include <time.h>       
#include <unistd.h>
#include "funciones.h"


/**
 * @brief Funcion principal
*/


int main(int arg, char *argv[])
{

	// Almacenar el tiempo de ejecución del código
	double time_spent = 0.0;
    clock_t begin = clock();
	FILE *file;  //archivo
	long long int bytes;
	int bloques;
	uint64_t s[25]={0}; 
	uint64_t s_uint64[25]={0};	//definir matrices en la pila para ahorrar tiempo
	uint8_t s_uint8[200]={0};
	uint8_t *in_string;
	//bits de salida
	int d = atoi(argv[4]); // obtener bits de salida
	file = fopen(argv[2],"rb");   //Se abre un archivo especificado por el usuario en modo lectura binaria (rb) utilizando 
	fseek (file,0,SEEK_END);	//Leer archivo para ver cuántos bytes tiene
	bytes = ftell (file);		//almacenar bytes de archivos
	rewind (file); 	
	bloques = (bytes/136); 		//# Número de bloques de 1088 bits (136 bytes)
	in_string = (uint8_t*)malloc (sizeof(uint8_t)*136*(1+bloques));	//array llena de bloques
	//guardar todo en un string
	fread(in_string,1,bytes,file);
	b2lendian(in_string,bytes);//Los datos en in_string se procesan para asegurar que estén en el orden correcto de bytes (endianess) utilizando la función b2lendian
	//Se aplica el padding requerido por el algoritmo Keccak
	padding(in_string,bytes,bloques);
	//Se llama la funcion sponge para calcular el hash Keccak del archivo. Los resultados se almacenan en la matriz s.
	sponge_keccakf(in_string, s_uint8,s_uint64,s,bloques);
	//se imprime el hash
	keyhex(s,d,25);
	//liberar espacio
	free(in_string);
	fclose(file);
	// calcula el tiempo transcurrido encontrando la diferencia (end - begin) y
    clock_t end = clock();
    // dividiendo la diferencia por CLOCKS_PER_SEC para convertir a segundos
	time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\n El tiempo transcurrido es de %f segundos", time_spent);
	return 0;
}
