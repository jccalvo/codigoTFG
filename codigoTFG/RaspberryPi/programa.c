/** @file programa.c
  *
  * @brief Fichero que contienr el programa principal.
  *
  * Este programa coge y lee de la plataforma los valores de la señal de referencia y de medida. -- json.c .
  * Posteriormente calcula la diferencia entre esos dos valores(error)y genera la señal de control. -- comparador.c y PID.c .
  * A continuación selecciona el comando de acuerdo con el valor de la señal de error. -- comandos.c .
  * Por último modula ese comando y lo transmite a través de la interfaz SPI. -- spi.c .
  *
  * @author Juan Carlos Calvo Sansegundo.  
  * @date 03/10/2016.
  */

#include <time.h>
#include <sys/time.h>
#include "json.h"
#include "comparador.h"
#include "PID.h"
#include "comandos.h"
#include "spi.h"

/**
  * Periodo de iteración del programa.
  */
#define PERIODOITER 10


/** @fn void* escribir_fichero(char *nombre, int diferencia).
  *
  * @brief Esta función se encarga de escribir en un fichero la diferencia 
  *  	   de temperatura que se ha calculado previamente.
  *  
  * @param  nombre. Nombre del fichero donde se va a escribir.
  * @param  diferencia. Valor que se va a escribir.   
  * @return No se devuelve nada si la operación se ha realizado correctamente
  *         En caso contrario se devuelve null;
  *  
  * @author Juan Carlos Calvo Sansegundo.   
  * @date 26/10/2016.
  */

 void* escribir_fichero(char *nombre,int diferencia){
   FILE * f;
   int dif= diferencia;
   if((f=fopen(nombre,"a+"))==NULL){
        printf("xxxxx> No se puede escribir en el fichero  %s \n\n",nombre);
        return NULL;
   }else{
   	fprintf(f,"-----> La diferencia de temp es: %d\n\n",dif); 
        fclose(f);  
        
   }
 }

 /** @fn void iteracion(struct timeval* current, struct timeval * next, struct timeval * timeout).
   *
   * @brief Esta función se encarga de controlar el periodo en el que se realiza la lectura de las 
   *         temperaturas de cada fichero.
   * 
   * @param current. Estructura timeval que almacena el instante actual.
   * @param next.    Estructura timeval que almacena el instante de la siguiente iteración. 
   * @param timeout. Estructura timeval que almacena el tiempo que falta hasta la siguiente iteración.
   *
   * @author Juan Carlos Calvo Sansegundo.   
   * @date 26/10/2016.
   */

 void iteracion(struct timeval* current, struct timeval * next, struct timeval * timeout){

    gettimeofday(current,NULL);
    next->tv_sec = next->tv_sec + PERIODOITER;

    timeout->tv_sec = next->tv_sec - current->tv_sec;
    timeout->tv_usec = next->tv_usec - current->tv_usec;
    if(timeout->tv_usec <0){
       	timeout->tv_usec = timeout->tv_usec + 1000000;
        timeout->tv_sec = timeout->tv_sec -1;
    }
       
    select(0,NULL,NULL,NULL,timeout); 
 }


 /** fn main()
   *
   * @brief Método principal de ejecución del programa.
   *
   * @author Juan Carlos Calvo Sansegundo.   
   * @date 26/10/2016.
   */

 int main(){
    struct dato_leido* ref;        // Estructura que contiene el valor de referencia.
    struct dato_leido* medida;     // Estructura que contiene el valor de medida.
    
    int error = 0;                 // Valor de la señal de error en n;
    int error_1 = 12280;              // Valor de la señal de error en (n-1);
    int error_2 = 34360;              // Valor de la señal de error en (n-2);
    int control = 0;               // Valor de la señal de control en n;
    int control_1 = 0;             // Valor de la señal de control en (n-1);


    struct timeval current_iteration = {0,0};
    struct timeval next_iteration = {0,0};
    struct timeval timeout = {0,0};

    gettimeofday(&current_iteration,NULL);
    gettimeofday(&next_iteration,NULL);

    int a = 0;

    while(1){
	  gettimeofday(&current_iteration,NULL);
          printf("--->Inst entrada: %10.2f seg y %10.2f mseg:\n\n",(double)current_iteration.tv_sec,(double)current_iteration.tv_usec);

          ref = extraer_dato(URL_SIZE,URL_FORMAT,TAM_BUFFER); 
          ref->temp=14000;
          medida = extraer_dato(URL_SIZE,URL_FORMAT,TAM_BUFFER); 
        
          if(ref==NULL || medida==NULL){
               fprintf(stderr,"xxxxx> Error: No se ha podido extraer el dato \n");
               return 1;
          }

          error = calcular_diferencia(ref->temp,medida->temp);
          escribir_fichero("temp_dif.txt",error);

          control = calcular_control(error,error_1,error_2,KP,KI,KD,PERIOD,control_1);
          printf("---> Valores de error: En n: %d. En n-1: %d. En n-2: %d.\n",error,error_1,error_2);
          printf("---> Valores de control: En n: %d. En n-1: %d.\n",control,control_1);

          control_1 = control;
          error_2 = error_1;
          error_1 = error;

          int p = seleccionar_comando(control);
          printf("Comando elegido: %d\n",p);
          struct buf_salida * res = crear_comando(p);
          struct buffer_comando * buffer = modular_comando(p);
        while(1){  
        p = transmitir_SPI(buffer);
        }   
         //iteracion(&current_iteration,&next_iteration,&timeout);
         // a++;
    }
 }


