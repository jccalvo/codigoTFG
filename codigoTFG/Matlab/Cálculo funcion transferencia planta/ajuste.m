%% -- Script ajuste.m
%
% -- En este script se extraen las funciones obtenidas de cada uno de los 
% -- experimentos realizados. 
% -- También se realizará un ajuste de todas las funciones estimadas en un
% -- mismo experimento, para determinar cuál de ellas es la que mejor se
% -- aproxima.
%
% -- Nombre: Juan Carlos Calvo Sansegundo.    Fecha: 18/01/2017.
%

%% -- Paso 0. Obtención de los resultados de la estimación.

a = 0;
while(a==0)
    nombre = input('------> Por favor introduzca el nombre de la carpeta donde se encuentran los datos: ','s');
    nombre = strcat('./',nombre);
    if(exist(nombre,'dir')==7.0)
        cd (nombre);
        load datos_experimento;
        fprintf('------> Datos abiertos correctamente. <------\n\n');
        a = 1;
    else
        fprintf('\n\n xxxxxx El directorio %s no existe. Vuelva a intentarlo. xxxxxx\n\n',nombre);
        a = 0;
    end;
end;
clearvars nombre a;

%% -- Paso 1. Extracción del error cuadrático normalizado de cada estimación.

coefs_ajuste =[];
for i=1:N_sistemas
    coefs_ajuste(end+1)=resultados{i}.Report.Fit.FitPercent;
end;

fprintf('------> Extracción de coeficientes realizada correctamente. <------\n\n');
%% -- Paso 2. Ordenación de las estimaciones de acuerdo a su coeficiente de ajuste.

for i=1:N_sistemas
    [max_coef,pos_max] = max(coefs_ajuste(1,i:N_sistemas));
    mejor = resultados{pos_max+i-1};
    resultados{pos_max+i-1}=resultados{i};
    resultados{i}=mejor;
    coefs_ajuste(1,pos_max+i-1)=coefs_ajuste(1,i);
    coefs_ajuste(1,i)=max_coef;
end;
[max_coef,pos_mejor] = max(coefs_ajuste);
mejor = resultados{pos_mejor};

clearvars i j  maximo pos_max  

fprintf('------> Ordenación realizada correctamente. <------\n\n');
%% -- Paso 3. Guardar la mejor estimación del experimento para hacer la correlación.

cd ../

carpeta = input('------> Introduzca la ruta de la carpeta en la que se quiere almacenar los datos: ','s');
carpeta = strcat('./',carpeta);
if(exist(carpeta,'dir')==7)
    cd(carpeta);
    load N.mat;
else
    mkdir (carpeta);
    cd(carpeta);
    N_exp = 0;
end

nombre = input('------> indique el nombre del experimento: ','s');
datos = struct('name',nombre,'best',mejor,'data',datos_estimacion,'fit',max_coef,'setpoint_init',setpoint_init,'setpoint_final',setpoint_final);
save (nombre,'datos');
N_exp = N_exp+1;
save ('N','N_exp');
fprintf('------> Datos guardados correctamente. <------ \n\n');
cd ../
clear;
