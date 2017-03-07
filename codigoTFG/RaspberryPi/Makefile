all:
	gcc -c json.c
	gcc -c comparador.c
	gcc -c PID.c
	gcc -c comandos.c
	gcc -c spi.c
	gcc -c programa.c
	gcc -o programa json.o comparador.o PID.o comandos.o spi.o programa.o -ljansson -lcurl -lrt -lpigpio



