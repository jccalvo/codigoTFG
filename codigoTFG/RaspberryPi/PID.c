/** @file PID.c
  *
  * @brief Fichero que contiene las funciones necesarias para describir
  * 	   el funcionamiento de un controlador PID discreto.
  *
  * @author Juan Carlos Calvo Sansegundo.   
  * @date 24/02/2017.
  */

#include "PID.h"

/** @fn int calcular_control(int err, int err_1, int err_2, int kp, int ki, int kd, int period, int control_1).
  *
  * @brief Esta función simula el comportamiento de un controlador PID discreto,
  * 	  calculando el valor de la señal de control, a partir de la señal de error
  *        que hay a la entrada del controlador.
  *
  * @param err.       Valor de la señal de error en el instante n.
  * @param err_1.     Valor de la señal de error en el instante n-1.
  * @param err_2.     Valor de la señal de error en el instante n-2.
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

int calcular_control(int err, int err_1, int err_2, int kp, int ki, int kd, int period, int control_1){

   // Definición de parte entera y parte decimal. La parte entera está expresada en unidades y la decimal en milésimas.

   int kp_ent = kp/FACT_CONV;
   int kp_dec = kp%FACT_CONV;
    
   int ki_ent = ki/FACT_CONV;
   int ki_dec = ki%FACT_CONV;

   int kd_ent = kd/FACT_CONV;
   int kd_dec = kd%FACT_CONV;

   int per_ent = period/FACT_CONV;
   int per_dec = period%FACT_CONV;

   int prop_ent, intr_ent, der_ent, total_ent;
   int prop_dec, intr_dec, der_dec, total_dec;
   int prop, intr, der, total;
   int total_control;

   int a,b,num; // Variables auxiliares.

   // Cálculo de la componente proporcional --> kp*(e(n)-e(n-1))=(kp_ent + kp_dec)((e(n)-e(n-1))ent + (e(n)-e(n-1))dec);

   prop_ent = kp_ent*(err - err_1)/FACT_CONV; // Con precision de unidades.
   prop_dec = (kp_ent*(err - err_1)%FACT_CONV + kp_dec*(err - err_1)/FACT_CONV + (kp_dec*(err - err_1)%FACT_CONV)/FACT_CONV);
   prop = prop_ent*FACT_CONV + prop_dec;

   // Cálculo de la componente integral --> Ki*T*e(n) =(ki_ent + ki_dec)*(T_ent + T_dec)*(e(n)ent e(n)dec);

   a = (ki_ent*per_ent);
   b = (ki_ent*per_dec) + (ki_dec*per_ent) + ((ki_dec*per_dec)/FACT_CONV);

   intr_ent = a*(err/FACT_CONV);
   intr_dec = (a*(err%FACT_CONV)+ b*(err/FACT_CONV)+(b*(err%FACT_CONV)/FACT_CONV));
   intr = intr_ent*FACT_CONV + intr_dec;

   /* Cálculo de la componente derivativa --> Kd*(e(n) -2*e(n-1) + e(n-2))/T. Presenta 2 fases:
    *
    * 1º Fase: Cálculo del numerador: num = Kd*(e(n)-2*e(n-1)+e(n-2)) = (kd_ent + kd_dec)*((e(n)-2e(n-1)+e(n-2))ent+(e(n)-2e(n-1)+e(n-2))dec);
    * 
    * 2º Fase: División con el periodo: num*FACT_CONV/period. (se multiplica el num por el factor para obtener precisión de milésimas).
    */

   a = kd_ent*((err-2*err_1+err_2)/FACT_CONV);
   b = kd_ent*((err-2*err_1+err_2)%FACT_CONV) + kd_dec*((err -2*err_1 +err_2)/FACT_CONV) + (kd_dec*((err -2*err_1 +err_2)%FACT_CONV)/FACT_CONV);
   num = a * FACT_CONV + b;

   der_ent = (num*FACT_CONV/period)/FACT_CONV;
   der_dec = (num*FACT_CONV/period)%FACT_CONV;
   der = der_ent*FACT_CONV + der_dec;
    
   total_ent = prop_ent + intr_ent + der_ent;
   total_dec = prop_dec + intr_dec + der_dec;
   total = total_ent * FACT_CONV + total_dec;
 
   total_control = control_1 + total_ent*FACT_CONV + total_dec;

   printf("-----> Comp_proporcional: %d.%d Comp_integral: %d.%d Comp_der: %d.%d \n",prop/FACT_CONV,abs(prop%FACT_CONV),intr/FACT_CONV,abs(intr%FACT_CONV),der/FACT_CONV,abs(der%FACT_CONV));
   printf("-----> Total_control: %d.%d\n",total_control/FACT_CONV,abs(total_control%FACT_CONV));
   return total_control;
}


