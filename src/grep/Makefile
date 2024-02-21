all: main.c structs.o
	gcc main.c struct.o -Wall -Wextra -Werror -o s21_grep

structs.o: structs.h structs.c
	gcc -c struct.h structs.c -Wall -Werror -Wextra -o structs.o