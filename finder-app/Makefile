# Target executablke name
TARGET = writer

# Source and object files
SRC = writer.c
OBJ = $(SRC:.c=.o)

# Default compiler and flags
CC ?= gcc
CFLAGS ?= -Wall -Werror

# If CROSS_COMPILE is provided prefix it to the compiler
ifdef CROSS_COMPIEL
	CC = $(CROSS_COMPILE)gcc
endif

# Default target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(TARGET) $(OBJ)
