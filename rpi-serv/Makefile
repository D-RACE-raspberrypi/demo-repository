CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = serveur_pi
SRC = src/server.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)