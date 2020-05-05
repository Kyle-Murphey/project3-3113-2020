all: project3

project3.o: project3.c
	gcc -c project3.c

project3: project3.o
	gcc project3.o -o project3

clean:
	rm -f *.o project3
