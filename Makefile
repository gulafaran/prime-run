 CC=gcc
VERSION=v1.0
ARCH=x86-64
BIN=prime-run

INCLUDE=-Iinclude/
PKGFLAGS=freeglut gl libpci
CFLAGS=-march=$(ARCH) -mtune=native -g $(INCLUDE) `pkg-config --cflags $(PKGFLAGS)`
LINKER=-lm `pkg-config --libs $(PKGFLAGS)`
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
DEPS = $(wildcard include/*.h)

primetest: $(OBJ)
	$(CC) $(OBJ) $(LINKER) -o src/$(BIN)

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

install:
	install -D -m755 src/$(BIN) $(DESTDIR)/usr/bin/$(BIN)

.PHONY: uninstall
uninstall:

.PHONY: clean
clean:
	rm src/*.o src/$(BIN)
