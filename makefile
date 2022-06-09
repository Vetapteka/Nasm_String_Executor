all: prog

prog: prog.o lib.o dict.o
	ld prog.o lib.o dict.o  -o prog

prog.o: main.asm
	nasm -felf64 main.asm -o prog.o

lib.o: lib.asm
	nasm -felf64 lib.asm -o lib.o

dict.o: dict.asm
	nasm -felf64 dict.asm -o dict.o

clean:
	rm -rf *.o prog


