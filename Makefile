# Feito Por Gabriel Lüders
#   GRR20190172 

# Variaveis
CFLAGS = -Wall
LDLIBS = -lm
CC = gcc 
OBJ = main.o dir.o image.o matrix.o utils.o


# regra default e ligação
all: mosaico

mosaico: $(OBJ)
	$(CC) -o mosaico $(OBJ) $(LDLIBS)

# Regras de compilação

main.o: main.c image.h utils.h
	$(CC) -c main.c $(CFLAGS)

dir.o: dir.c dir.h utils.h
	$(CC) -c dir.c $(CFLAGS)

image.o: image.c image.h matrix.h dir.h utils.h
	$(CC) -c image.c $(CFLAGS)

matrix.o: matrix.c matrix.h 
	$(CC) -c matrix.c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c utils.c $(CFLAGS)


# Regras de limpeza

clean: 
	-rm -f *.o

purge: clean
	-rm -f mosaico