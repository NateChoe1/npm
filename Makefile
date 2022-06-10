SRC = $(wildcard src/*.c)
OBJ = $(subst .c,.o,$(subst src,work,$(SRC)))
CFLAGS := -O2 -pipe -Wall -Wpedantic -Wshadow -ansi
CFLAGS += -Isrc/include/
INSTALLDIR := /usr/bin/
OUT = npm

build/$(OUT): $(OBJ)
	$(CC) $(OBJ) -o build/$(OUT)

work/%.o: src/%.c $(wildcard src/include/*.h)
	$(CC) $(CFLAGS) $< -c -o $@

install: build/$(OUT)
	cp build/$(OUT) $(INSTALLDIR)/$(OUT)

uninstall: $(INSTALLDIR)/$(OUT)
	rm $(INSTALLDIR)/$(OUT)
