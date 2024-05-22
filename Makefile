CC=g++
CFLAGS=-O3 -std=c++26 -Dunix
CXXFLAGS=-O3 -std=c++26 -Dunix -fconcepts-diagnostics-depth=2

SRC:= Torus.cpp\
	  $(wildcard Conscreen/*.c)\
	  $(wildcard Conscreen/List/*.c)

OBJ_TMP:= $(SRC:.c=.o)
OBJ:= $(OBJ_TMP:.cpp=.o)

Torous: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)
clean:
	rm -rf $(OBJ) Torous
echo:
	echo $(OBJ)
