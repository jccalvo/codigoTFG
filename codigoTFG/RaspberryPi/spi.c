/** @file spi.c
  *
  * @brief Fichero que contiene las funciones necesarias para usar la interfaz SPI
  *        de la raspberry PI y poder enviar los comandos al aire acondicionado a través  
  *        de dicha interfaz.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 03/11/2016.
  */

#include "spi.h"


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

struct buffer_comando * modular_comando(int temperatura){

     unsigned int t_bit;      // Periodo de bit (useg).
     unsigned int f_bit;      // Frecuencia de bit (baudios).

     unsigned int t_carrier;  // Periodo de portadora (useg).
     unsigned int f_carrier;  // Frecuencia de portadora (Hz).

     unsigned int nciclos;     // Número de ciclos de la portadora.

     unsigned int long_bit_modulado;   // Longitud de un bit del comando modulado.
     unsigned int long_comando;	       // Longitud total en bits del comando modulado.
     unsigned int bits_parada;         // Bits de parada de las transiciones SPI.
     unsigned int long_comando_bytes;  // Longitud del comando en bytes.
     unsigned int despl_izq;           // Desplazamiento de bits a la izquierda para ordenar el comando.
     unsigned int despl_der;           // Desplazamiento de bits a la derecha para ordenar el comando.
     unsigned int bits_de_exceso;      // Bits sobrantes por el uso de los bits de parada.

     struct buffer_comando * buffer;   // Estructura que almacena el comando modulado.

     // Creación del comando.
     struct buf_salida * comando = crear_comando(temperatura); 
     if(comando ==NULL){
	  printf("xxxxx> Error: Comando no disponible.\n");
          return NULL;
      }

      t_bit = (comando->datos[BYTE_PER_BIT1]<<TAM_PAL_BITS)+comando->datos[BYTE_PER_BIT2];   // Extracción del tiempo de bit del comando.
      f_bit =  CONV_SEG_USEG/t_bit;                       // Cálculo de la frecuencia de bit.
 
      printf("-----> Periodo de bit: %d useg.\n",t_bit);
      printf("-----> Frecuencia de bit: %d baudios.\n\n",f_bit);

      t_carrier = comando->datos[BYTE_PER_CARRIER];          // Extracción del periodo de la portadora.
      f_carrier = CONV_SEG_USEG/t_carrier;    // Cálculo de la frecuencia de portadora.
      
      printf("-----> Periodo de portadora: %d useg.\n",t_carrier);
      printf("-----> Frecuencia de portadora: %d Hz\n\n",f_carrier);

      f_bit =2*f_carrier;

     // Creación del array que almacena la señal de salida modulada.
     nciclos =(t_bit/t_carrier);
     if(t_bit%t_carrier!=0){
	  nciclos+=1;
     }
     printf("-----> Numero de ciclos: %d.\n",nciclos);
     printf("-----> Numero de palabras: %d\n",comando->datos[BYTE_NPALABRAS]);
     
     long_bit_modulado = 2*nciclos;
     printf("-----> Longitud del bit modulado: %d.\n",long_bit_modulado);
     
     long_comando = long_bit_modulado*(comando->datos[BYTE_NPALABRAS])*TAM_PAL_BITS;
     printf("-----> Longitud del comando en bits: %d.\n",long_comando);   

     bits_parada = long_comando/TAM_PAL_BITS;
     printf("-----> Número de bits de parada: %d.\n",bits_parada);

     long_comando_bytes = bits_parada;
     printf("-----> Longitud del comando en bytes: %d.\n",long_comando_bytes);

     despl_izq = long_comando%TAM_PAL_BITS;
     if(despl_izq!=0){
	long_comando_bytes += 1; 
     }
     printf("-----> Desplz a la izq en bits: %d.\n",despl_izq);

     despl_der = TAM_PAL_BITS*long_comando_bytes - long_comando;
     printf("-----> Desplz a la der en bits: %d.\n",despl_der);

     bits_de_exceso = bits_parada + despl_der; 
     printf("-----> Bits en exceso: %d.\n\n",bits_de_exceso);

     
     // Modulación del comando.
     int i = 0;
     int j = 0;
     int marcador = 0;

     unsigned char mascara = MASCARA_BITS;
     buffer = (struct buffer_comando*)calloc(1,sizeof(struct buffer_comando));
     buffer->puntero = (unsigned char*)calloc(long_comando_bytes,sizeof(unsigned char));
     buffer->posicion = j;
     buffer->f_bit = f_bit;


     for(i=BYTE_INI_DATOS;i<(comando->datos[BYTE_NPALABRAS]*TAM_PAL_BITS + BYTE_INI_DATOS);i++){
         for(marcador=0; marcador<long_bit_modulado;marcador++){
             if(comando->datos[i]==CHAR_0){
		 buffer->puntero[j]= buffer->puntero[j] & (~mascara);
             }else if(comando->datos[i] ==CHAR_1){
                 if(marcador%2 ==0 || marcador ==0){
                     buffer->puntero[j]= buffer->puntero[j] | mascara;
                 }else{ 
                     buffer->puntero[j]= buffer->puntero[j] & (~mascara);  
                 }
             }else{
             }

             mascara = mascara >>1;
             if(!mascara){   
		 mascara = MASCARA_BITS;
                 j++;
                 buffer->posicion = j;
             }
         }
     } 
/*
     for(i=0;i<buffer->posicion;i++){
	 printf("--> Posición %d. 0x%x \n",i+1, buffer->puntero[i]);
     }*/
     printf("\n---> Modulación realizada. <----\n\n");

     free(comando->datos);
     free(comando);
    
     // Desplazamiento de los bits sobrantes del último byte al inicio.
     unsigned char a = 0;
     unsigned char b = 0;

     if(despl_der!=0){
      	 for(i=long_comando_bytes -1; i>0; i--){
	    a = buffer->puntero[i] >>  despl_der;
            b = buffer->puntero[i-1] << despl_izq;
            buffer->puntero[i]= a|b;
         } 
         buffer->puntero[0]=buffer->puntero[0]>> despl_der;
    /*
         for(i=0;i<buffer->posicion;i++){
	    printf("-->Posición %d. 0x%x.\n",i+1, buffer->puntero[i]);
         }  
       */  printf("\n-----> Desplazamiento de ceros realizado.\n\n");    
     }

     // Eliminación de los bits sobrantes por el uso de los bits de parada del spi.
     int pal_inicio =long_comando_bytes-1;
     int p =0;
     int k = 0;     

     while(k<long_comando_bytes){
        while(p < long_bit_modulado/TAM_PAL_BITS){
           for(i=pal_inicio;i>=0;i--){
               a = buffer->puntero[i] >> 1;
               b = buffer->puntero[i-1] <<7;
               buffer->puntero[i]= a|b;
           }
           p++;
           k++;
        }
        pal_inicio = pal_inicio-long_bit_modulado/TAM_PAL_BITS;
        p=0;
     }

     //buffer->puntero[4] = 0x01;
     /*for(i=0;i<buffer->posicion;i++){
	    printf("--> Posición %d. 0x%x \n",i+1, buffer->puntero[i]);
     }*/
     printf("\n-----> Eliminación de bits sobrantes hecha.\n\n");
     return buffer;
}


/** @fn transmitir_SPI(struct buffer_comando* buffer).
  *
  * @brief Función que se encarga de inicializar la librería PIGPIO, abrir los pines 
  *        de la interfaz SPI y realizar la transmisión de dicho comando.
  *
  * @param buffer. Puntero a la estructura que almacena el comando modulado.   
  * @return Devuelve 1 se ha realizado la transmisión correctamente. Devuelve 0 en caso contrario.
  *
  * @author Juan Carlos Calvo Sansegundo.    
  * @date 04/12/2016.
  */

int transmitir_SPI(struct buffer_comando* buffer){

    // Configuración de la interfaz SPI con PIGPIO.

    int library_version;     // Versión de la librería.
    int set_SCLK;            // Variable de control puerto SCLK.
    int set_MOSI;	     // Variable de control puerto MOSI.
    int set_MISO;	     // Variable de control puerto MISO.
    int set_CE;              // Variable de control puerto CE.
    int device;
    int i;
    unsigned int flag_pines;
    unsigned int channel = 0;
    unsigned int flags = 1;
    unsigned int nbytes= 1;
    

    // Carga de la librería para su uso. 
   
    library_version = gpioInitialise();

    if(library_version < OK){
	//errores_SPI(library_version);
        return -1;
    }
    else{
	printf("Librería cargada correctamente\n");
    }
  
    // Selección de los puertos GPIO a usar;
     
    set_SCLK = gpioSetMode(PORT_SCLK,OUTPUT);
    set_MOSI = gpioSetMode(PORT_MOSI,OUTPUT);
    set_MISO = gpioSetMode(PORT_MISO,INPUT);
    set_CE = gpioSetMode(PORT_CE,OUTPUT);

    flag_pines = (set_SCLK || set_MOSI || set_MISO || set_CE);

    if(flag_pines){

        if(set_SCLK !=OK){
          printf("xxxxx> Error: Pin SCLK no configurado correctamente.\n"); 
        }
        if(set_MOSI !=OK){
           printf("xxxxx> Error: Pin MOSI no configurado correctamente.\n"); 
        }
        if(set_MISO !=OK){
           printf("xxxxx> Error: Pin MISO no configurado correctamente.\n"); 
        }
        if(set_CE !=OK){
           printf("xxxxx> Error: Pin CE no configurado correctamente.\n"); 
        }
        return -1;     
    }
    else{ 
        printf("-----> Pines seleccionados correctamente. \n");
        printf("--> Puerto SCLK bien definido. Nº Puerto: %d y modo: %d\n",PORT_SCLK,gpioGetMode(PORT_SCLK));
        printf("--> Puerto MOSI bien definido. Nº Puerto: %d y modo: %d\n",PORT_MOSI,gpioGetMode(PORT_MOSI));
        printf("--> Puerto MISO bien definido. Nº Puerto: %d y modo: %d\n",PORT_MISO,gpioGetMode(PORT_MISO));
        printf("--> Puerto CE   bien definido. Nº Puerto: %d y modo: %d\n\n",PORT_CE,gpioGetMode(PORT_CE));
    }

    // Definición e inicialización de la interfaz SPI.

    printf("-----> Flags: 0x%x\n\n",flags);
    
    char palabra;
    while(1){
    for(i=0;i<buffer->posicion;i++){
    	device = spiOpen(channel,buffer->f_bit,flags);
        if(device < OK){
	    printf("xxxxx> Error: Dispositivo no creado correctamente.\n\n");
	    return -1;
        }
        palabra = buffer->puntero[i];
	spiWrite(device,&palabra,nbytes);
        spiClose(device);
    }
   }
    gpioTerminate();
    free(buffer->puntero);
    free(buffer);
    return OK;
}

