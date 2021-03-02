

unmix: unmixapp.o utils.o norm.o
	gcc -o unmix utils.o norm.o unmixapp.o -lm -lgsl -lgslcblas
unmixapp.o: unmixapp.c utils.h norm.h
	gcc -c unmixapp.c
utils.o: utils.c norm.h
	gcc -c utils.c
norm.o: norm.c
	gcc -c norm.c
