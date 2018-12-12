CC := gcc
CFLAGS := -std=c99

studer: serial.o studer.o main.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

.PHONY: clean
clean: 
	rm *.o studer
