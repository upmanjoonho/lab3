CC = gcc
CFLAGS = -Wall -fPIC
AR = ar
ARFLAGS = rcs
LD = gcc
LDFLAGS = -shared

SRC = lib/calc.c
OBJ = calc.o

TARGET = main

STATIC_LIB = lib/libcalc.a

DYNAMIC_LIB = lib/libcalc.so

.PHONY: all clean static_lib dynamic_lib

all: static_lib dynamic_lib $(TARGET)

$(TARGET): main.o $(STATIC_LIB)
	$(CC) -o $@ $^ -Llib -lcalc

main.o: main.c
	$(CC) -c $< -o $@

$(STATIC_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(DYNAMIC_LIB): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) main.o $(TARGET) $(STATIC_LIB) $(DYNAMIC_LIB)
