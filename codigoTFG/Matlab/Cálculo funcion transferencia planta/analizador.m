%% - Script analizador.m
%  - 
%  - Este script se encarga de realizar la correlación de las mejores
%  - funciones de cada experimento realizado para así poder determinar cuál
%  - de todas ellas es la mejor estimación de la función de transferencia.
%  -
%  - Nombre: Juan Carlos Calvo Sansegundo    Fecha = 05/02/2017.
%  -

%% - Paso 0: Carga de datos.

a = 1;
while (a~=0)
   carpeta = input('------> Introduzca la ruta de la carpeta en la que se encuentran los datos: ','s');
   carpeta =strcat('./',carpeta);
   if(exist(carpeta,'dir')==7.0)
      cd(carpeta)
      load N.mat
      fprintf('------> Acceso realizado correctamente. <------ \n\n');
      a = 0;
   else
      fprintf('xxxxxx Error: La carpeta no existe. Vuelva a intentarlo. xxxxxx\n\n');
   end;
end;


%% Extracción de datos 

datos_global=struct; % Struct que almacena las mejores funciones de cada experimento.
for p=1:N_exp
    nombre = strcat('Experimento',num2str(p),'.mat');
    load(nombre);
    datos_global= setfield(datos_global,strcat('Experimento',num2str(p)),datos);
end;
clearvars datos carpeta nombre a


%% Cálculo de la correlación de cada función de transferencia con el resto de experimentos.

corr_global = struct; % Struct que almacena la correlación de cada función con el resto de experimentos.
n = struct;           % struct que almacena los datos de cada iteración.
h = figure();
set(h,'Visible','off');

for p=1:N_exp
    nombre_sist = strcat('Experimento',num2str(p));
    sist = getfield(getfield(datos_global,nombre_sist),'best');
    for q=1:N_exp 
        nombre_data = strcat('Experimento',num2str(q));
        datos =getfield(getfield(datos_global,nombre_data),'data');
        [y,fit,xo]= compare(datos,sist);
        error = y.OutputData - datos.OutputData;
        errorMax = max(abs(error));
        hold on
        plot(errorMax,fit,'o');
        text(errorMax,fit+0.1,(strcat(num2str(p),',',num2str(q))));
        hold off;
        m = struct('y_est',y,'ajuste',fit,'xo',xo,'error',error,'errorMax',errorMax);
        n = setfield(n,strcat('Exp',num2str(p),'datos',num2str(q)),m); 
    end;
    corr_global =setfield(corr_global,strcat('Funcion',num2str(p)),n);
end;    

set(h,'Visible','on');
title('Diagrama de pareto de correlacion de funciones');
grid on;
xlabel('Máximo error');
ylabel('Coeficiente de ajuste(%)');

fprintf('------> Correlacion realizada correctamente. <------ \n\n');
clearvars nombre_sist nombre_data datos error errorMax m n y fit xo sist datos

%% Cálculo de parámetros estadísticos para seleccionar la mejor función 

coef_ajuste_propio =zeros(1,N_exp);
coef_ajuste_resto =zeros(N_exp,N_exp+1);
errorMax_propio=zeros(1,N_exp);
errorMax_resto=zeros(N_exp,N_exp+1);
error_propio=zeros(1,N_exp);
error_resto=zeros(N_exp,N_exp+1);


for p=1:N_exp
    for q=1:N_exp 
        parametros = getfield(corr_global,strcat('Funcion',num2str(p)));
        parametros = getfield(parametros,strcat('Exp',num2str(p),'datos',num2str(q)));
        if(p==q)
            coef_ajuste_propio(1,p)=parametros.ajuste;
            errorMax_propio(1,p)=parametros.errorMax;
            error_propio(1,p)=mean(abs(parametros.error));
            coef_ajuste_resto(p,q)=0;
            errorMax_resto(p,q)=0;
            error_resto(p,q)=0;
        else
            coef_ajuste_resto(p,q)=parametros.ajuste;
            errorMax_resto(p,q)=parametros.errorMax;
            error_resto(p,q)=mean(abs(parametros.error));
        end;
    end;
    coef_ajuste_resto(p,N_exp+1)=sum(coef_ajuste_resto(p,1:N_exp))/(N_exp-1.0);
    errorMax_resto(p,N_exp+1)=sum(errorMax_resto(p,1:N_exp))/(N_exp-1.0);
    error_resto(p,N_exp+1)=sum(error_resto(p,1:N_exp))/(N_exp-1.0);
end;
estadisticos = struct('ajuste_propio',coef_ajuste_propio,'errorMax_propio',errorMax_propio,'error_propio',error_propio,...
               'ajuste_resto',coef_ajuste_resto,'errorMax_resto',errorMax_resto,'error_resto',error_resto);
fprintf('------> Cálculo de estadísticos realizado correctamente. <------ \n\n');
%%clearvars errorMax_propio errorMax_resto coef_ajuste_propio coef_ajuste_resto error_propio error_resto

%% Selección de la mejor función;
best0 = {'Concepto' 'Función' 'Resultado'};

[a,b]= max(coef_ajuste_propio);
best1 ={'Mejor ajuste propio:'  num2str(b) num2str(a)};
[a,b]= max(coef_ajuste_resto(:,N_exp+1));
best2 ={'Mejor ajuste con el resto:'  num2str(b) num2str(a)};
[a,b]= min(errorMax_propio);
best3 ={'Mejor error Máximo propio:'  num2str(b) num2str(a)};
[a,b]= min(errorMax_resto(:,N_exp+1));
best4 ={'Mejor error Máximo con el resto'  num2str(b) num2str(a)}
[a,b]= min(error_propio);
best5 ={'Mejor error medio propio:'  num2str(b) num2str(a)}
[a,b]= min(error_resto(:,N_exp+1));
best6 ={'Mejor error medio con el resto:'  num2str(b) num2str(a)}

q = [best0;best1;best2;best3;best4;best5;best6];
Best = table(q);


fprintf('------> Cálculo de estadísticos realizado correctamente. <------ \n\n');
clearvars a b q best0 best1 best2 best3 best4 best5 best6 
%% Almacenamiento de datos

cd ../
carpeta = input('------> Introduzca la ruta de la carpeta en la que quiere los datos: ','s');
carpeta =strcat('./',carpeta);
if(exist(carpeta,'dir')==7.0)
   cd(carpeta);
else
   mkdir(carpeta);
   cd(carpeta);
end;
nombre = input('------> Introduzca el nombre del paquete en el que va a guardar los datos: ','s');
save (nombre,'datos_global','corr_global','N_exp','Best','h','estadisticos');
fprintf('------> Datos guardados correctamente. <------ \n\n');
  
cd ../

set (h,'Visible','off');
clear;
