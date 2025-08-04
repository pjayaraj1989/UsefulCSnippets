# Makefile for AES-GCM Multi-Update Example

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcrypto
TARGET = aes_gcm_multi
SRC = *.c

all: $(TARGET)

$(TARGET): $(SRC)
$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
rm -f $(TARGET)
