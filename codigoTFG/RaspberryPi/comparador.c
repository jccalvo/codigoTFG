/** @file comparador.c
  *
  * @brief Fichero que contiene las funciones necesarias para 
  * 	   realizar la comparación entre la señal de referencia
  * 	   y la señal de medida.
  *
  * @author Juan Carlos Calvo Sansegundo.     
  * @date 24/02/2017.
  */

#include "comparador.h"

/** @fn int calcular_diferencia(int valor_ref, int valor_med).
  *
  * @brief Esta función cálcula la diferencia entre la señal de referencia
  * 	   y la señal de medida.
  *
  * @param valor_ref. Valor de temperatura de la señal de referencia.
  * @param valor_med. Valor de temperatura medido por el sensor.
  * @return Un entero con la diferencia de las 2 temperaturas.
  *
  * @author Juan Carlos Calvo Sansegundo.     
  * @date 24/02/2017.
  */

int calcular_diferencia(int valor_ref, int valor_med){

  int valor_err = valor_ref - valor_med;
  return valor_err;
}
