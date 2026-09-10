CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
TARGET = serveur_pi
SRC = src/server.c src/car.c src/moteur.c src/servo.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)