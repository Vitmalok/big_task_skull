main: make/Image.o
	gcc -o main src/main.c make/Image.o make/lodepng.o
make/Image.o: src/Image.c make/lodepng.o
	gcc -o make/Image.o -c src/Image.c
make/lodepng.o: src/lodepng.c
	gcc -o make/lodepng.o -c src/lodepng.c
