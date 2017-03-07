/**  @file json.c
  *
  *  @brief Fichero que contiene las funciones necesarias para realizar la
  *  solicitud de petición HTTP al servidor y la obtención del objeto JSON,
  *  así como su almacenamiento y posterior tratamiento.
  *
  *  @author Juan Carlos Calvo Sansegundo.   
  *  @date 20/10/2016.
  */

#include "json.h"

/**  @fn char * crear_memoria(int tam_memoria).
  *
  *  @brief Esta función se encarga de crear una memoria dinámica donde
  *  se almacena el objeto JSON.
  *
  *  @param tam_memoria. Tamaño de la memoria.
  *  @return Un puntero de tipo char que almacena la dirección 
  *  	     en la que se encuentra. En caso contrario devuelve null.
  *
  *  @author Juan Carlos Calvo Sansegundo.
  *  @date 20/10/2016.
  */

  char * crear_memoria(int tam_memoria){

     char * memoria;
     memoria =(char*)calloc(tam_memoria,sizeof(char));

     if(memoria == NULL){
	 fprintf(stderr,"xxxxx> Error: No se ha creado correctamente la memoria \n");
         return NULL;
     }else{
         return memoria;
     }
  } 


/**  @fn size_t escribir_respuesta(void *ptr, size_t size, size_t nmemb, void *stream, int tam_memoria).
  *
  *  @brief Esta función escribe el objeto json en la memoria y comprueba que el tamaño
  *  de la memoria es suficiente.
  *
  *  @param ptr. Puntero de tipo void.
  *  @param size. Estructura de tipo size_t que almacena el tamaño del objeto.
  *  @param nmemb. Estructura de tipo size_t que contiene el número de posiciones del objeto.
  *  @param stream. Puntero de tipo void que se refiere al flujo de datos a escribir.
  *  @param tam_memoria. Tamaño de la memoria de almacenamiento.
  *  @return Estructura de tipo size_t con el número de posiciones que ocupa el objeto  
  *  	     En caso de no poder escribir, devuelve un 0.
  *
  *  @author Juan Carlos Calvo Sansegundo.  
  *  @date 20/10/2016.
  *
  */

  size_t escribir_respuesta(void *ptr, size_t size, size_t nmemb, void *stream, int tam_memoria){
     struct escribir_resultado *result = (struct escribir_resultado *)stream;

     if(result->posicion + size * nmemb >=tam_memoria - 1){
        fprintf(stderr, "xxxxx> Error: Búffer demasiado pequeño.\n");
        return 0;
     }

     memcpy(result->datos + result->posicion, ptr, size * nmemb);
     result->posicion += size * nmemb;

     return size * nmemb;
  }


/**  @fn char * solicitar_http(const char *url, int tamano_buffer).
  *  
  *  @brief Esta función se encarga de realizar la petición http al servidor y obtener el objeto JSON.
  *
  *  @param url. Puntero a un array de caracteres con la URL del objeto JSON.
  *  @param tamano_buffer. Tamaño del buffer de almacenamiento.
  *  @return Un puntero a la memoria donde se almacena el objeto JSON.
  *  	     En caso contrario devuelve null.
  *
  *  @author Juan Carlos Calvo Sansegundo.  
  *  @date 20/10/2016.
  */

  char * solicitar_http(const char * url,int tamano_buffer){
      
     CURL * curl = NULL;   // Creación de un puntero a una estructura de tipo curl.
     CURLcode estado;      // Guarda el estado sobre la conexión http.
     long codigo;          // Almacena el código HTTP sobre el resultado de la petición.

     curl_global_init(CURL_GLOBAL_ALL);
     curl=curl_easy_init();               // Creación del manejador de funciones de librería curl.
 
     if(!curl){
        fprintf(stderr,"xxxxx> Error: No se ha podido inicializar el curl\n");
        curl_global_cleanup();
        return NULL;
     }

     // Creación de la estructura que maneja el buffer.
     struct escribir_resultado resultado={
	.datos = crear_memoria(tamano_buffer),
        .posicion = 0
     };

     // Comprobación de que se ha creado el buffer.
     if(resultado.datos == NULL){
         fprintf(stderr,"xxxxx> Error: No se ha creado bien el buffer\n");
         free(curl);
         curl_global_cleanup();
         return NULL;
     }
     
     // Asignación de url, método de escritura y zona de almacenamiento al objeto curl.
     curl_easy_setopt(curl,CURLOPT_URL,url);
     curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,escribir_respuesta);
     curl_easy_setopt(curl,CURLOPT_WRITEDATA,&resultado);
     estado = curl_easy_perform(curl);

     // Comprobación de que se ha realizado la conexión http.
     if(estado !=CONEXION_OK){
	fprintf(stderr,"xxxxx> Error: No se puede conectar con la url: %s\n",url);	
        fprintf(stderr,"xxxxx> %s\n",curl_easy_strerror(estado));
        free(resultado.datos);
        free(curl);
        curl_global_cleanup();
        return NULL;
     }

     // Obtención del resultado de la petición http.
     curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &codigo);
     if(codigo !=PET_HTTP_OK){
        fprintf(stderr,"xxxxx> Error: El servidor ha respondido con el código %ld\n",codigo);
	free(resultado.datos);
        free(curl);
        curl_global_cleanup();
        return NULL;
     }

     // Cierre del objeto curl.
     curl_easy_cleanup(curl);
     curl_global_cleanup();
     
     resultado.datos[resultado.posicion]='\0'; // Indica el final del objeto JSON.
     printf("-----> Conexión correcta \n\n");
     
     return resultado.datos;
  }


/**  @fn struct dato_leido * extraer_dato(int url_size, const char* url_format, int tamano_buffer).
  *  
  *  @brief Esta función se encarga de extraer la temperatura del objeto JSON.
  *
  *  @param url_size. Tamaño de la url del objeto JSON.
  *  @param url_format. Puntero de tipo char con el formato de la url.
  *  @param tamano_buffer. Tamaño del buffer para almacenar el objeto JSON.  
  *  @return Un puntero a una estructura (definida como dato_leido) que contiene
  *	     el valor de la temperatura y el instante de medida. Sino devuelve NULL.
  *
  *  @author Juan Carlos Calvo Sansegundo.  
  *  @date 25/10/2016.
  */


  struct dato_leido * extraer_dato(int url_size,const char* url_format, int tamano_buffer){

     char * texto;  // Puntero que almacena la dirección del buffer.
     char url[url_size];
     size_t i = 0;

     // Creación e inicialización de la estructura a devolver.
     struct dato_leido * resultado = (struct dato_leido*)calloc(1,sizeof(struct dato_leido));
     resultado->temp = 0;
     resultado->instante = 0;
   
     json_t* obj_json;      // Maneja el objeto json .
     json_error_t errores;  // Maneja los errores que se den al manejar el objeto JSON.
  
     json_t* array_temps;   // Maneja el array de datapoints.
     json_t* temp_json;     // Maneja la temperatura del datapoint.
     json_t* inst_json;     // Maneja el instandte de medida del datapoint.

   
     int a = snprintf(url, url_size, url_format);

     // Solicitud de la petición http.
     texto = solicitar_http(url, tamano_buffer);
     if(texto == NULL){
	fprintf(stderr, "xxxxx> Error: No se ha podido conectar con el servidor.\n");
        return NULL;
     }

     // Carga del objeto json global y eliminación del buffer.
     obj_json = json_loads(texto,0,&errores);    
     if(!obj_json){
	 fprintf(stderr, "xxxxx> Error: en la línea %d: %s\n", errores.line, errores.text);
         free(texto);
         return NULL;
     }
     free(texto);

     // Extracción del contenido del objeto json global.
     obj_json = json_array_get(obj_json,0);
     if(!json_is_object(obj_json)){
         fprintf(stderr, "xxxxx> Error: No hay objeto.\n");
         return NULL;
     }

     // Extracción del datapoint(temperatura) del objeto JSON.
     obj_json = json_object_get(obj_json,"datapoints");
     if(!json_is_array(obj_json)){
         fprintf(stderr, "xxxxx> Error: No existen el array de datapoints\n");
         return NULL;
     }
     
     for(i=0;i<json_array_size(obj_json);i++){
           
           int inst;
           int temp;	

           array_temps = json_array_get(obj_json,i);
           temp_json = json_array_get(array_temps,0);
           inst_json = json_array_get(array_temps,1);

           if(!json_is_number(temp_json) && !json_is_number(inst_json)){
		fprintf(stderr,"xxxxx> Error: Fallo al leer la temperatura y el instante de medida.\n");
                continue;
           }else if (!json_is_number(temp_json)){
		fprintf(stderr,"xxxxx> Error: Fallo al leer la temperatura.\n\n");
                continue;
           }else if (!json_is_number(inst_json)){
		fprintf(stderr,"xxxxx> Error: Fallo al leer el instante de medida.\n\n");
                continue;
           }else{
           }
          
           inst = (int)json_integer_value(inst_json);
           temp = json_real_value(temp_json)*FACT_CONV_TEMP;
           
           // Se comprueba que la muestra es la más reciente.
           if(inst >= resultado->instante){
		resultado->temp = temp;
                resultado->instante = inst;
           }

           printf("-----> Extracción correcta \n");
           printf("-----> Temperatura leída: %d.%d\n",resultado->temp/FACT_CONV_TEMP,resultado->temp%FACT_CONV_TEMP);
           printf("-----> Instante_lectura: %d\n\n",resultado->instante);
           
     }

     printf("-----> Resultado final \n");
     printf("-----> Temperatura leída: %d.%d\n",resultado->temp/FACT_CONV_TEMP,resultado->temp%FACT_CONV_TEMP);
     printf("-----> Instante_lectura: %d\n\n",resultado->instante);

     return resultado;
  }
