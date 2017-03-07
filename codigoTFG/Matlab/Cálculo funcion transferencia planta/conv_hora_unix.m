function [fecha, hora] = conv_hora_unix(datos)

if(isempty(datos))
    fprintf('\n------ Por favor introduzca fecha y hora con el formato: [dd/mm/aaaa] [hh:mm:ss]. ------\n');
    dia = input('dia: ');
    mes = input('mes: ');
    anyo = input('año: ');
    hra =input('hora: ');
    min = input('min: ');
    seg = input('seg: ');
else
    dia = datos(1,3);
    mes = datos(1,2);
    anyo = datos(1,1);
    hra = datos(1,4);
    min = datos(1,5);
    seg = floor(datos(1,6));
end    
Lim_max_dia = 0;

% Definición de constantes:
INICIO_UNIX = [01 01 1970]; % Instante de referencia para la hora unix.
CONV_HRAS_SEG =3600;        % Conversión de horas a segundos.
CONV_MIN_SEG = 60;          % Conversión de minutos a segundos.
HRAS_DIA = 24;              % Número de horas que tiene un día;
DIAS_ANYO = 365;            % Número de días que tiene un año.
ANYO_REF_BIS = 1972;        % Primer año bisiesto en el formato unix.
MIN_MAX=60;                 % Máximo número de minutos.
SEG_MAX = 60;               % Máximo número de segundos.

% Comprobación de que la fecha introducida es correcta.

switch(mes)
    case {1,3,5,7,8,10,12} 
        Lim_max_dia = 31;
    case {4,6,9,11} 
        Lim_max_dia = 30;
    case 2
        if(rem(anyo-ANYO_REF_BIS,4)==0)
            Lim_max_dia = 29;
        else
            Lim_max_dia = 28;
        end;
    otherwise
        fprintf('Error: Mes no válido. Introduzca un mes entre 1 y 12(ambos inclusive)\n');
        return;
end;

if(dia<0 || dia>Lim_max_dia)
    fprintf('Error: Día no válido. Introduzca un día acorde con el mes\n');
    return;
elseif(anyo<INICIO_UNIX(1,3))
    fprintf('Error: Año no válido. Introduzca un año mayor o igual que 1970\n');
    return;
else
    fprintf('\n------ Fecha introducida correctamente. ---------\n');
end;

% Comprobación de que la hora introducida es válida.

if(hra<0 || hra>HRAS_DIA)
    fprintf('Error: Hora no válida. Introduzca un valor entre 0 y 23 (ambos inclusive).\n');
    return;
elseif(min<0 || min>MIN_MAX)
    fprintf('Error: Minuto no válido. Introduzca un valor entre 0 y 59(ambos inclusive).\n');
    return;
elseif(seg<0 || seg>SEG_MAX)
    fprintf('Error: Segundos no válidos. Introduzca un valor entre 0 y 59(ambos inclusive).\n');
    return;
else
    fprintf('\n------ Hora introducida correctamente. ---------\n\n');
end;

% Confirmación de datos.

 fprintf(' -------- La fecha y hora introducida son: ---------\n')
 fprintf(' -------- Fecha: %d/%d/%d. -------\n',dia,mes,anyo);
 fprintf(' -------- Hora: %d:%d:%d. -------\n\n',hra,min,seg);
 
% Conversión de la hora del formato estándar al formato unix.
hora_unix_1 = (anyo-INICIO_UNIX(1,3))*DIAS_ANYO*HRAS_DIA*CONV_HRAS_SEG;  % Años completos.
hora_unix_2 = floor((anyo-ANYO_REF_BIS)/4 + 1)*HRAS_DIA*CONV_HRAS_SEG; % Días años bisietos.
hora_unix_3 = 0;

for i=1:mes-1
    switch(i)
    case {1,3,5,7,8,10,12} 
        hora_unix_3 = hora_unix_3 + 31;
    case {4,6,9,11} 
        hora_unix_3 = hora_unix_3 + 30;
    case 2
        if(rem(anyo-1972,4)==0)
            hora_unix_3 = hora_unix_3 + 29;
        else
            hora_unix_3 = hora_unix_3 + 28;
        end;
    otherwise
        hora_unix_3 = hora_unix_3 + 0;
    end;
end;

hora_unix_3 = (hora_unix_3+ dia-1)*HRAS_DIA*CONV_HRAS_SEG; % Dias transcurridos en el año actual.
hora_unix_4 = (hra-1)*CONV_HRAS_SEG + min*CONV_MIN_SEG + seg; % Segundos transcurridos en el día actual.Se pone en Horario canario.

hora_unix_total = hora_unix_1 + hora_unix_2 + hora_unix_3 + hora_unix_4;

fprintf('La conversion a formato unix es: %14.2f\n\n',hora_unix_total);

fecha = [dia mes anyo];
hora = [hra min seg hora_unix_total];
end










