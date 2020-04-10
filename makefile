

unmix: beta.o
	gcc -o unmix beta.o -lm -lgsl -lgslcblas
beta.o: beta.c
	gcc -c beta.c
