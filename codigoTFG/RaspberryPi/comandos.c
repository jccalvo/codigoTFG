/** @file comandos.c
  * 
  * @brief  Fichero que contiene las funciones necesarias para que a partir
  *  	    de la señal obtenida a la salida del controlador PID se pueda
  *         generar el comando que se quiere transmitir al aire acondicionado.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 30/10/2016.
  */

#include "comandos.h"

/** @fn int seleccionar_comando(int temp_sal_PID).
  *
  * @brief Esta función coge el resultado obtenido a la salida del controlador 
  * 	   y establece una correspondencia de dicho valor, con los posibles valores
  *        de temperatura que dispone el aire acondicionado.
  * 
  * @param temp_sal_PID. Temperatura de salida del controlador PID en centésimas.
  * @return Un valor de temperatura dentro del rango aceptado por el aire acondicionado.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 30/10/2016.
  */

int seleccionar_comando(int temp_sal_PID){

     int temp; 

     // Redondeo y conversión a unidades.
     temp = (temp_sal_PID + LIMITE_REDONDEO)/FACT_CONV;

     // Selección de la temperatura a aplicar al aire acondicionado.
     if(temp>=TEMP_MAXIMA_AC){
	 return TEMP_MAXIMA_AC;
     }
     else if(temp<=TEMP_MINIMA_AC){
	 return TEMP_MINIMA_AC;
     }
     else{
	 return temp;  
     }
}

/** @fn FILE* elegir_fichero(int temperatura, FILE *f)
  *
  * @brief Esta función se encarga de generar el nombre del fichero que 
  *        almacena el comando a enviar, de acuerdo con la temperatura
  *        generada por el controlador PID.
  *
  * @param temperatura. Temperatura a la que hay que poner el aire acondicionado.
  * @param f. Puntero de tipo file usado para abrir el comando de temperatura.
  * @return Un puntero de tipo FILE con el comando seleccionado. 
  *
  * @author Juan Carlos Calvo Sansegundo.   
  * @date 30/10/2016.
  */

FILE* elegir_fichero(int temperatura,FILE *f){
    
    char url[URL_SIZE_FILE];
    int a = snprintf(url, URL_SIZE_FILE, URL_FORMAT_FILE,temperatura);
    f = fopen(url,"rb");
    return f;
}


/** @fn buf_salida * crear_comando(int temperatura).
  *
  * @brief Esta función coge el fichero de texto asociado
  *        la temperatura obtenida,genera el comando y lo transmite 
  *        al módulo de infrarrojos.
  *  
  * @param temperatura. Contiene la temperatura a enviar.
  * @return Un struct de tipo buf_salida con el comando generado.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 30/10/2016.
  */

struct buf_salida * crear_comando(int temperatura){

    FILE * f = NULL;                   // Puntero al fichero del comando.
    unsigned char buffer[800];         // Buffer de almacenamiento del fichero del comando.
    unsigned char mask = MASCARA_BITS; // Máscara de conversión de caracteres a bits.
    unsigned int j = 0;
    int i = 0;
    size_t n;
    
    // Creación de la estructura que contiene el comando generado.
    struct buf_salida* salida = (struct buf_salida*)calloc(1,sizeof(struct buf_salida));
    salida->datos = (unsigned char*)calloc(800,sizeof(unsigned char));
     
    // Selección del fichero de texto que contiene el comando y copia del contenido en el buffer.   
    f = elegir_fichero(temperatura,f);
    if(f == NULL){
       printf("xxxxx> Error: El comando seleccionado no existe\n");
       return NULL;
    }
    n = fread(buffer, 1, sizeof(buffer), f);
    printf("-----> Número de bloques leídos: %u\n",n);
    fclose(f);

    // Generación del comando (Solo se pasan a bits los caracteres de la cabecera).
    for(i=0;i<n;i++){
       if(i < BITS_CABECERA){
	   if(buffer[i]==CHAR_1){ 
	      salida->datos[j] |= mask;
              salida->posicion = j;
           }
           else if(buffer[i]==CHAR_0){
	      salida->datos[j] &=~mask;
              salida->posicion = j;
           }
           else{
           }

           mask >>=1;
           if(!mask){
	       mask=MASCARA_BITS;
	       j++;
           }   
       }else{
	   salida->datos[j]=buffer[i];
           salida->posicion = j;
           ++j;
       }

    }

    //salida->datos[j++]='\n';
    salida->posicion=j;

    for(i=0;i<salida->posicion;i++){
	printf("Byte %d:",i);
        printf("0x%x\n",salida->datos[i]);
    }
    printf("\n -----> Número de bytes: %d\n",salida->posicion);
    return salida;
}

