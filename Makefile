# Makefile for compiling a Raylib project in Linux

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -std=c99

# Raylib and other libraries
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Source files
SRC = inter.c

# Output executable
TARGET = inter

# Default rule
all: $(TARGET)

# Rule to link object files and create the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean rule to remove the executable
clean:
	rm -f $(TARGET)

# Phony targets to avoid conflicts with files named 'all' or 'clean'
.PHONY: all clean

EXAMPLE_IMAGE_SRC = image_example.c 
EMAMPLE_IMAGE_TGT = image_example 

img_example: 
	$(CC) $(CFLAGS) -o img_example image_example.c $(LIBS)