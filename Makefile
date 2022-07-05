default:
	clang --target=wasm32 -nostdlib -O0 -g -std=c11 -Wall -Werror --pedantic \
		-flto -Wl,--no-entry \
		-Wl,--allow-undefined \
		-Wl,--import-memory \
		src/ssmm.c \
		src/ext/doom/z_zone.c src/ext/doom/doomdef.c \
		-o ssmm.wasm

test: src/main.c src/tree.c
	clang -O2 -g -std=c11 -Wall -Werror --pedantic -D TEST \
		src/main.c src/tree.c -o test && ./test
