CC = gcc
CFLAGS = -Wall
SOURCES = server.c
TARGET = server

default: $(TARGET)
all: default

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
