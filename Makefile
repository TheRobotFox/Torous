CC=g++
CFLAGS=-O3 -std=c++26 -Dunix

SRC:= Torus.cpp\
	  $(wildcard Conscreen/*.c)\
	  $(wildcard Conscreen/List/*.c)

OBJ:= $(SRC:.c=.o)

Torous: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)
clean:
	rm -rf $(OBJ) Torous
echo:
	echo $(OBJ)
