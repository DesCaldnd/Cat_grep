all: main.c structs.o
	gcc main.c structs.o -Wall -Wextra -Werror -o s21_grep

s21_grep: main.c structs.o
	gcc main.c structs.o -Wall -Wextra -Werror -o s21_grep

structs.o: structs.h structs.c
	gcc -c structs.h structs.c -Wall -Werror -Wextra

clean:
	rm *.o
	rm *.gch
	rm s21_grep