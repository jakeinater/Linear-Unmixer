#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#include "utils.h"
#include "norm.h"

/*
 * * Program that reads reference spectra and measure spectra data
 * and linearly unmixes the data using method of least squares fit.
 * Assumes readings is set to 400 and file path is preset to:
 *  
 * syntax: ./linearUnmixer MUfilename ref1filename ref2filename ref3filename ...
 *  returns: completion status, prints composition of ref1, ref2, ... , refn
 * 
 *  * ****************************************************************
 *  * Author       Dept.           Date            Notes
 *  * ****************************************************************
 *  * Jake Z       Bio. Eng.       Mar 12 2020     Initial version
 *  * Jake Z            "          Apr 03 2020     It works now! 
 *  TODO: investigate the discrepancy between what you set $readings and the amount 
 *  of space you allocate to the matrices as you forgot to account for the initial 
 *  data clearing of the title headers in the data.
 * */

const int readings = 1000;	//num of data points in each reading
const int skip = 50;		//num of data points to discard from beginning

int main(int argc, char *argv[])
{
	if ( argc <= 2 )
	{
		fprintf( stderr, "error, incorrect usage.\n correct usage: ./unmix unknown ref1 ref2 ... refn\n");
		return 3;
	}

	//allocate memory for vars
	int numRefs = argc - 2;
	char *filePath = "./assets/";	// create prompt/arg for desired assets

	gsl_vector *muVector = gsl_vector_alloc(readings);
	gsl_vector *fVector = gsl_vector_alloc(readings);
	gsl_vector *x = gsl_vector_alloc(numRefs);

	gsl_matrix *refMatrix = gsl_matrix_alloc(readings, numRefs);
	gsl_matrix *C = gsl_matrix_alloc( numRefs, numRefs );
	gsl_matrix *Cinverse = gsl_matrix_alloc( numRefs, numRefs );

	int s;
	gsl_permutation *p = gsl_permutation_alloc(numRefs);

	//initialize vars to 0
	
	gsl_vector_set_zero(muVector);
	gsl_vector_set_zero(fVector);
	gsl_vector_set_zero(x);
	gsl_matrix_set_zero(refMatrix);
	gsl_matrix_set_zero(C);
	gsl_permutation_init(p);	//INITIALISED THE PERMUTATION

	readFileVector( muVector, filePath, argv[1] );	//read vals of unknown arg
	readFileMatrix( refMatrix, filePath, argc, argv );	//read the refs into matrix

	normalizeVector( muVector );

	//create fn that reads the files

	FILE *csvfile = fopen( "./muVector.csv", "wt" );
	gsl_vector_fprintf( csvfile, muVector, "%f" );
	fclose(csvfile);
	
	FILE *csvfile2 = fopen( "./refVectors.txt", "wt" );
	gsl_matrix_fprintf( csvfile2, refMatrix, "%f" );
	fclose(csvfile2);

	//now we have variable muVector with vals and refMatrix with vals.
	
	gsl_blas_dgemm( CblasTrans, CblasNoTrans, 1, refMatrix, refMatrix, 0, C);       //matrix C is now A^T A
	gsl_blas_dgemv( CblasTrans, 1, refMatrix, muVector, 0, fVector);        //fVector is 'b'
	gsl_linalg_LU_decomp( C, p, &s);
	gsl_linalg_LU_invert( C, p, Cinverse );         

	//TODO maybe implement the other way you can solve for basis using inverse on both sides, (A^T A )^-1 A^T A x = (a^t a)^-1 (a^t b) = x
        
	gsl_blas_dgemv( CblasNoTrans, 1, Cinverse, fVector, 0, x);
	
	gsl_vector_fprintf( stdout, x, "%f" );

	gsl_matrix_free(Cinverse);
	gsl_matrix_free(C);
	gsl_matrix_free(refMatrix);
	gsl_vector_free(muVector);
	gsl_vector_free(fVector);
	gsl_vector_free(x);

	return 0;
}
