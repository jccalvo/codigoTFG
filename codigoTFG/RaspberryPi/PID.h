/**  @file PID.h
  *
  *  @brief Este fichero contiene las cabeceras de las funciones necesarias 
  *         que describen el funcionamiento de un controlador PID discreto.
  *
  *  @author Juan Carlos Calvo Sansegundo.   
  *  @date 24/02/2017.
  */

#ifndef PID_H
#define PID_H

#include <stdio.h>
#include <stdlib.h>

/** 
  * Constante proporcional expresado en milésimas.
  */
#define KP 1000   
/** 
  * Constante integral expresado en milésimas.
  */  
#define KI 1000    
/** 
  * Constante derivativa expresado en milésimas.
  */   
#define KD 1000
/** 
  * Periodo de muestreo expresado en milésimas.
  */      
#define PERIOD 10000  

/** 
  * Factor de conversion de unidades a milésimas y viceversa.
  */      
#define FACT_CONV 1000 

/** @fn int calcular_control(int err, int err_1, int err_2, int kp, int ki, int kd, int period, int control_1).
  *
  * @brief Esta función simula el comportamiento de un controlador PID discreto,
  * 	   calculando el valor de la señal de control, a partir de la señal de error
  *        que hay a la entrada del controlador.
  *
  * @param err.     Valor de la señal de error en el instante n.
  * @param err_1.   Valor de la señal de error en el instante n-1.
  * @param err_2.   Valor de la señal de error en el instante n-2.
  * @param kp.        Constante proporcional del controlador.
  * @param ki.        Constante integral del controlador.
  * @param kd.        Constante derivativa del controlador.
  * @param period.    Periodo de muestreo.
  * @param control_1. Valor de la señal de control en el instante n-1.
  * @return Valor de la señal de control en el instante n.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 24/02/2017.
  */

int calcular_control(int error, int error_1, int error_2, int kp, int ki, int kd, int period, int control_1);

#endif
