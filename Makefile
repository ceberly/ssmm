default:
	clang --target=wasm32 -nostdlib -O0 -g -std=c11 -Wall -Werror --pedantic \
		-Wl,--no-entry -Wl,--export-all, -Wl,--allow-undefined \
		src/tree.c src/doom.c \
		src/ext/doom/z_zone.c src/ext/doom/doomdef.c \
		-o ssmm.wasm

test: src/main.c src/tree.c src/ssmm.h
	clang -O2 -g -std=c11 -Wall -Werror --pedantic -D TEST \
		src/main.c src/tree.c -o test && ./test
