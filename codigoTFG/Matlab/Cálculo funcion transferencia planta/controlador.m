%% Script controlador.m
%  Este script se encarga de calcular la función de transferencia de la
%  planta a controlar (aire acondicionado) y del controlador de la planta (controlador PID). 
%
%  Para ello, se conecta con la plataforma que contiene los datos de
%  entrada y de salida, se extraen dichos datos y una vez se tengan esos
%  datos, se procederá al uso de la función ftest, para calcular la función
%  de transferencia de la planta.
%
%  Nombre: Juan Carlos Calvo Sansegundo.   Fecha: 19/12/2016.
%  

% -- Paso 0.0: Limpieza del espacio de trabajo del programa.
clc; 
clear;
disp('------> Espacio de trabajo limpio.');
fprintf('\n\n');


% -- Paso 0.1: Definición de variables.

FACT_CONV_PER = 10;     % Factor de conversión del periodo.

IN_temp=[];             % Array de temperaturas de la entrada.
IN_inst=[];             % Array de instantes de tiempo de la entrada.
            
IN_temp_muestr=[];      % Variable que almacena la señal de entrada muestreada.
IN_inst_muestr=[];      % Variable que almacena los instantes de la señal de entrada muestreada.

OUT_temp=[];            % Array de temperaturas de la salida.
OUT_inst=[];            % Array de instantes de tiempo de la salida.

OUT_JSON=[];            % Variable que almacena el objeto JSON de la salida.
OUT_JSONdatapoint=[];   % Variable que almacena los datapoints de la salida.
OUT_temp_muestr=[];     % Variable que almacena la señal de salida muestreada.
OUT_inst_muestr=[];     % Variable que almacena los instantes de la señal de salida muestreada.
OUT_muestra=[];         % Variable auxiliar que almacena una muestra de temperatura del datapoint de entrada.

datos =[];              % Variable para convertir la hora en formato unix.

fprintf('------> Por favor introduzca la dir http de la señal de salida: ')
OUT_url=input(' ','s');   % URL de almacenamiento de valores de la señal de salida.
    
fprintf('------> Por favor introduzca el periodo de muestreo deseado: ');
Ts = input(' ');     % Periodo de muestreo(debe ser un número >0 y entero). (Es 10 segundos). 

fprintf('------> Por favor introduzca el setpoint inicial fijado: ');
setpoint_init = input('');
fprintf('------> Por favor introduzca el setpoint final fijado: ');
setpoint_final = input('');

disp('------> Introduzca la fecha y hora del instante de inicio de toma de muestras.');
[fecha_init,hora_init] = conv_hora_unix(datos);

disp('------> Introduzca la fecha y hora del instante final de toma de muestras.');
[fecha_final,hora_final] = conv_hora_unix(datos);

% -- Diferencia de tiempo desde el instante inicial hasta el instante actual
datos = clock;
[fecha_actual,hora_actual] = conv_hora_unix(datos);
from = num2str(hora_actual(1,4) - hora_init(1,4));
OUT_url = strcat(OUT_url,'&from=-',from,'seg');

fprintf('------> Por favor introduzca el orden máximo del sistema: ');
orden = input(' ');

% -- Paso 0.2: Admisión de datos.

fprintf('xxxxxx Los datos introducidos son los siguientes: xxxxxx\n');
fprintf('xxxxxx URL salida:  %s xxxxxxx\n',OUT_url);
fprintf('xxxxxx Periodo de muestreo: %d segundos. xxxxxx\n',Ts);
fprintf('xxxxxx Setpoint inicial fijado: %d. xxxxxx\n',setpoint_init);
fprintf('xxxxxx Setpoint final fijado: %d. xxxxxx\n',setpoint_final);
fprintf('xxxxxx Inicio toma de muestras: [%d/%d/%d], [%d:%d:%d] [%14.2f formato_unix]. xxxxxx\n',fecha_init(1),fecha_init(2),fecha_init(3),hora_init(1),hora_init(2),hora_init(3),hora_init(4));
fprintf('xxxxxx Final toma de muestras: [%d/%d/%d], [%d:%d:%d] [%14.2f formato_unix]. xxxxxx\n',fecha_final(1),fecha_final(2),fecha_final(3),hora_final(1),hora_final(2),hora_final(3),hora_final(4));
fprintf('xxxxxx Hora actual: [%d/%d/%d], [%d:%d:%d] [%14.2f formato_unix]. xxxxxx\n',fecha_actual(1),fecha_actual(2),fecha_actual(3),hora_actual(1),hora_actual(2),hora_actual(3),hora_actual(4));
fprintf('xxxxxx Orden máximo del sistema %d. xxxxxx\n\n',orden);

flag = ' ';
while(flag~='0' && flag~='1')
    disp('------> Si desea continuar pulsa 1. En caso contrario pulsa 0 ');
    flag = input('------> ','s');
    if(flag == '0')
        fprintf('xxxxxx Salida del programa. xxxxxx\n');
        clear;
        return
    elseif(flag=='1')
        time_init = hora_init(4);
        time_final = hora_final(4);
        fprintf('xxxxxx Inicio de estimación de función de transferencia realizado. xxxxxx\n');
    else
    end;
end;

clearvars fecha_actual hora_actual fecha_init hora_init fecha_final hora_final datos flag from

% -- Paso 1. Generación de la señal del objeto JSON de la salida.

   % try
        OUT_JSON = webread(OUT_url);
        OUT_JSONdatapoint = OUT_JSON.datapoints;     % Extracción del objeto JSON de salida.
%    catch
%        warning('xxxxx Error: No se puede obtener objeto de salida');
%        warning('Revise la dir http, la conexión o la estructura del objeto JSON xxxxxx'); 
%        fprintf('\n\n');
%        return;
%     end;

    [OUT_Nfilas, OUT_Ncolumnas]=size(OUT_JSONdatapoint); % Obtiene el nº de filas y columnas del datapoint de salida.

fprintf('-------> Obtención de objeto JSON de entrada y salida realizada. \n\n');    
clearvars OUT_url OUT_JSON OUT_Ncolumnas 

% -- Paso 2. Parseo del objeto JSON de la salida.

if(iscell(OUT_JSONdatapoint))     % La extracción del dato depende de si el datapoint es cell o double.
    for i=1:OUT_Nfilas
        OUT_muestra = OUT_JSONdatapoint{i};
        [OUT_muestra_Nfil,OUT_muestra_Ncol]=size(OUT_muestra);  % Comprueba si el valor no es NULL y está dentro del rango de tiempos.
        if(OUT_muestra_Ncol~=2 ||(OUT_muestra(2)<time_init || OUT_muestra(2)>time_final))    
            continue;
        else
           OUT_temp(end+1)=OUT_muestra(1,1);
           OUT_inst(end+1)=OUT_muestra(1,2);
        end;
    end;
elseif(isnumeric(OUT_JSONdatapoint))
    for i=1:OUT_Nfilas
        OUT_muestra =[OUT_JSONdatapoint(i,1) OUT_JSONdatapoint(i,2)];
        if(OUT_muestra(2)<time_init || OUT_muestra(2)>time_final)
           continue;
        else
           OUT_temp(end+1)=OUT_muestra(1,1);
           OUT_inst(end+1)=OUT_muestra(1,2);
        end;
    end;
else
    fprintf('xxxxxx El objeto no tiene un formato válido. Fin del programa. xxxxxx \n\n'); 
    return;
end;

fprintf('------> Extracción de objeto JSON de salida realizada. \n\n');   
clearvars OUT_JSONdatapoint OUT_Nfilas OUT_muestra_Nfil OUT_muestra_Ncol OUT_muestra; 
 
% -- Paso 3. Parseo del objeto JSON de la entrada.

[IN_Nfilas,IN_Ncolumnas]= size(OUT_temp); 
for i=1:IN_Ncolumnas
     p = linspace(setpoint_init,setpoint_final,12);
     if(i>=1 && i<=90)
         IN_temp(end+1)=setpoint_init; 
     elseif(i>=91 && i<=102)
         IN_temp(end+1)=p(i-90);
     else
         IN_temp(end+1)=setpoint_final;
     end;
     IN_inst(end+1)=OUT_inst(i);
end;
fprintf('------> Extracción de objeto JSON de entrada realizada. \n\n');   
clearvars  IN_Nfilas IN_Ncolumnas; 

% -- Paso 4. Muestreo de la señales de entrada y salida.

if(mod(Ts,10)~=0 | Ts <=0)
    fprintf('xxxxxx Periodo de muestreo no válido.Debe ser un múltiplo de 10. Fin del programa. xxxxxx \n\n');
    return;
else
    fprintf('------> Periodo válido. \n\n');
    Ts = Ts/FACT_CONV_PER;
    Nmuestras = min(size(IN_temp)/Ts,size(OUT_temp)/Ts);
    for i=1:Nmuestras(2)
          IN_temp_muestr(end+1)= IN_temp(i*Ts);
          IN_inst_muestr(end+1)= IN_inst(i*Ts);
          OUT_temp_muestr(end+1)= OUT_temp(i*Ts);
          OUT_inst_muestr(end+1)= OUT_inst(i*Ts);
    end;    
end;

fprintf('------> Muestreo de entrada y salida realizado. \n\n');   
clearvars IN_temp IN_inst OUT_temp OUT_inst Nmuestras i time_init time_final;
    
% -- Paso 5. Cálculo de la función de transferencia estimada.    

IN_temp_muestr= IN_temp_muestr.';
OUT_temp_muestr = OUT_temp_muestr.';

plot(IN_temp_muestr)
hold on
plot(OUT_temp_muestr)
hold off


if ((size(IN_temp_muestr)==0) | (size(OUT_temp_muestr)==0))
    fprintf('xxxxxx Error: El array está vacío. Revise el rango de tiempos permitido. Fin del programa. xxxxxx \n\n');
    return;
end;

datos_estimacion = iddata(OUT_temp_muestr,IN_temp_muestr,Ts*FACT_CONV_PER)

fprintf('------> Creación de objeto iddata para estimación hecho. \n\n');   
clearvars IN_inst_muestr IN_temp_muestr OUT_inst_muestr OUT_temp_muestr FACT_CONV_PER Ts


% -- Paso 6.1. Determinación de número de combinaciones posibles según el orden.
% -- Paso 6.2 Creación de combinaciones posibles.
% -- Se crea un array de pares y cada par contiene el número de polos y ceros
% -- de esa combinación.

% combinaciones =[
% [polos_c1,ceros_c1],[polos_c2,ceros_c2],..[polos_cn,ceros_cn]]
%

N_sistemas = 0;
combinaciones=[];
for i=0:orden;
     for j=0:i
         N_sistemas = N_sistemas + 1;
         par =[i j];
         combinaciones = vertcat(combinaciones,par);
     end;
end;

fprintf('------> Cálculo de combinaciones posibles de polos y ceros hecho. \n\n');   
% -- Paso 6.3. Estimación de funciones de transferencia según las combinaciones

resultados = struct;
for i=1:N_sistemas
    p = strcat('est_',num2str(i));
    par =[combinaciones(i,1), combinaciones(i,2)]; 
    value = tfest(datos_estimacion,par(1,1),par(1,2));
    resultados = setfield(resultados,p,value);
end;
resultados = struct2cell(resultados);

fprintf('------> Estimación de la función de transferencia para cada combinación hecho. \n\n');   
clear vars i j p value par 

% -- Paso 7. Se almacenan las estimaciones de todas las combinaciones.
 
a = 0;
while(a==0)
    fprintf('------> Por favor introduzca el nombre de la carpeta donde guardar los datos. \n');
    nombre = input('------> ','s');
    nombre = strcat('./',nombre);
    if(exist(nombre,'dir')==7.0)
       fprintf('\nxxxxxx Error: El nombre ya existe. Introduzca otro diferente. xxxxxx \n\n');  
        a = 0;
    else
        clearvars par a 
        mkdir(nombre);
        cd(nombre);
        save datos_experimento
        fprintf('\nxxxxxx Los datos se han guardado correctamente en %s. xxxxxx\n' ,nombre);
        fprintf('------> Estimación realizada correctamente. \n');
        cd('../');
        a = 1;
    end;
end;

clear;