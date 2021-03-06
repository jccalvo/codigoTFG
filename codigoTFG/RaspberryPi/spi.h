/** @file spi.h
  *
  * @brief Fichero que contiene las cabecaeras de las funciones para usar la interfaz SPI
  *  	   de la raspberry PI y poder enviar los comandos al aire acondicionado a través  
  *        de dicha interfaz.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 02/11/2016.
  */

#ifndef SPI_H
#define SPI_H

#include <pigpio.h>
#include <string.h>
#include "comandos.h"

/**
  * Puerto SCLK en Raspberry PI
  */
#define PORT_SCLK 11

/**
  * Puerto MOSI en Raspberry PI.
  */
#define PORT_MOSI 10

/**
  * Puerto MISO en Raspberry PI.
  */
#define PORT_MISO  9

/**
  * Puerto CE en Raspberry PI.
  */
#define PORT_CE    8

/**
  * El puerto es configurado como salida.
  */
#define OUTPUT 1

/**
  * El puerto es configurado como entrada.
  */
#define INPUT  0

/**
  * Todo correcto.
  */ 
#define OK 0

/**
  * Factor de conversion de segundos a microsegundos.
  */
#define CONV_SEG_USEG 1000000

/**
  * 1º byte de datos del comando.
  */
#define BYTE_INI_DATOS 10

/**
  * Número de bits en un byte.
  */
#define TAM_PAL_BITS    8

/**
  * Byte del comando que contiene el número de bytes de datos.
  */
#define BYTE_NPALABRAS 6

/**
  * 1º Byte del comando que contiene el periodo de bit
  */
#define BYTE_PER_BIT1 7

/**
  * 2º Byte del comando que contiene el periodo de bit
  */
#define BYTE_PER_BIT2 8

/**
  * Byte del comando que contiene el periodo de la portadora
  */
#define BYTE_PER_CARRIER 9




/** @struct buffer_comando.
  *  
  * @brief Estructura que almacena el comando modulado.
  *
  * @author Juan Carlos Calvo Sansegundo.     
  * @date 04/12/2016.
  */

struct buffer_comando{
   unsigned char* puntero; /**< Puntero a la zona de memoria donde está almacenado.*/
   int posicion;           /**< Número de palabras que ocupa el comando modulado.*/ 
   int f_bit;              /**< Frecuencia de bit.*/
};


/** @fn buffer_comando * modular_comando(int temperatura).
  *
  * @brief Función que genera el comando a transmitir por la interfaz SPI. Extrae los parámetros
  *        de transmisión(frecuencia de bit, frecuencia de portadora...) y genera ese comando de
  *        acuerdo con dichos parámetros. 
  *
  * @param temperatura. Temperatura a la que hay que poner el aire acondicionado.
  * @return Devuelve 1 si se ha hecho la modulación correctamente. Devuelve 0 en caso contrario.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 04/12/2016.
  */

struct buffer_comando* modular_comando(int temperatura);


/** @fn transmitir_SPI(struct buffer_comando* buffer).
  *
  * @brief Función que se encarga de inicializar la librería PIGPIO, abrir los pines 
  *         de la interfaz SPI y realizar la transmisión de dicho comando.
  * 
  * @param buffer. Puntero a la estructura que almacena el comando modulado.   
  * @return Devuelve 1 se ha realizado la transmisión correctamente. Devuelve 0 en caso contrario.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 04/12/2016.
  */
int transmitir_SPI(struct buffer_comando * buffer);

#endif
