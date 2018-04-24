
CC=gcc
CFLAGS=-W -Wall -D_REENTRANT
LDFLAGS=-lpthread 
PATH_C=sources/c/
PATH_H=sources/h
PATH_OBJ=obj/
PATH_DELIVERY=delivery/

SRC=$(wildcard $(PATH_C)*.c)
OBJ=$(SRC:$(PATH_C)%.c=$(PATH_OBJ)%.o)
 
TARGET=$(PATH_DELIVERY)collector

all: mrproper $(TARGET)

$(TARGET): $(OBJ) 
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ):$(PATH_OBJ)%.o: $(PATH_C)%.c
	$(CC) -I$(PATH_H) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(PATH_OBJ)*.o
	
mrproper: clean
	rm -rf $(TARGET)
		
		
