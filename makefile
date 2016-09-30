DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.cpp)  
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

TARGET = main

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++
CFLAGS = -g -Wall -I${DIR_INC} -std=c++11

${BIN_TARGET}:${OBJ}
	    $(CC) $(OBJ)  -o $@ -lpthread
		    
${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	    $(CC) $(CFLAGS) -c  $< -o $@ -lpthread

clean:
	find ${DIR_OBJ} -name *.o -exec rm -rf {} \;
	rm bin/main
