shell: shell.c
	gcc -o shell shell.c -I.
shell2: shell2.c
	gcc -o shell2 shell2.c -I.

a: a.c
	gcc -o a1 a.c -I.

clean: 
	rm shell a1
