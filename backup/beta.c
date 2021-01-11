#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include </usr/local/include/gsl/gsl_matrix.h>
#include <gsl/gsl_matrix.h> // suprisingly, both work
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

/*
 *  * Program that reads reference spectra and measure spectra data
 *  and linearly unmixes the data using method of least squares fit.
 *  Assumes readings is set to 400 and file path is preset to:
 *  "/mnt/c/Users/jakei/'OneDrive - McGill University'/'Year 2'/1BIEN210/'Group Project'/".
 *  
 * syntax: ./linearUnmixer MUfilename ref1filename ref2filename ref3filename ...
 * returns: completion status, prints composition of ref1, ref2, ... , refn
 *
 *  * ****************************************************************
 *  * Author       Dept.           Date            Notes
 *  * ****************************************************************
 *  * Jake Z       Bio. Eng.       Mar 12 2020     Initial version
 *  * Jake Z		"	   Apr 03 2020     It works now! TODO: investigate the discrepancy between what you set $readings and the amount of space you allocate to the matrices as you forgot to account for the initial data clearing of the title headers in the data. 
 *  * */
const int READINGS = 1300;		//note, if the readings specified here is more then the number of readings in file, funky stuff happens, TODO: implent check for this
const int SKIP = 50;
int normalizeVector( gsl_vector *vector )
{
	double min = gsl_vector_min( vector );
	double norm = gsl_vector_max( vector ) - min;
	gsl_vector_add_constant( vector, -min );
	gsl_vector_scale( vector, 1/norm );
	return 0;
}

int readFileVector( gsl_vector *vector, char *filePath, char *fileName )		//read file vals into vector
{
	char tmpFilepath[120] = "\0";
	char tmpReading[20];
	char *tmp2;
	double reading;					//WARNING this is similar to global var $readings, consider renaming

	strcat( tmpFilepath, filePath );		//TODO replace this and the first instance of strcat into strcpy, that should work
	strcat( tmpFilepath, fileName ); 	//combines the filepath and name into one searchable /path/file
	FILE *file = fopen( tmpFilepath, "rt");
	printf("%s\n", tmpFilepath);			//TODO DELETE THIS
	if ( file == NULL ) {			//check if file exists
		fprintf( stderr, "Error, unable to locate the Mu data file\n");
		exit(100);
	}

	for ( int i = 0; i < SKIP; i++)		//move past header of read files
	{
		fgets( tmpReading, 19, file );
	}
	for ( int i = 0; i < READINGS; i++ )
	{
		fgets( tmpReading, 19, file );
	
		tmp2 = strrchr( tmpReading, '\t' );
		tmp2 = tmp2 + 1;
		reading = atof(tmp2);
		gsl_vector_set( vector, i, reading);
	}
	fclose(file);
	printf("successful vector \n");			//TODO delete this
	return 0;
}

int readFileMatrix( gsl_matrix *matrix, char *filePath, int argc, char *argv[] )
{
	char *tmp2;
	double reading;
	for ( int j = 2; j < argc; j++ )
	{
		gsl_vector_view column;
		char tmpFilepath[120] = "\0";				//TODO replace this and the first instance of strcat into strcpy, that should work
		char tmpReading[20];
		strcat( tmpFilepath, filePath );
		strcat( tmpFilepath, argv[j] ); 	//combines the filepath and name into one searchable /path/file
		FILE *file = fopen( tmpFilepath, "rt");
		printf("%s\n", tmpFilepath);				//TODO DELETE THIS
		if ( file == NULL ) {			//check if file exists
			fprintf( stderr, "Error, unable to locate a reference data file\n");
			exit(100);
		}
		for ( int i = 0; i < SKIP; i++)		//move past header of read files
		{
			fgets( tmpReading, 20, file);
		}
		for ( int i = 0; i < READINGS; i++ ) //fill the ith element of the jth column
		{
			fgets( tmpReading, 19, file );
		
			tmp2 = strrchr( tmpReading, '\t' );
			tmp2 = tmp2 + 1;
			reading = atof(tmp2);
			gsl_matrix_set( matrix, i, j-2, reading );
		}
		fclose(file);
		column = gsl_matrix_column( matrix, j-2 );				//NORMALISE EACH REFERENCE VECOTR.
		normalizeVector( &column.vector );		
	}
	printf("success, matrix filled \n");			//TODO DELETE THIS
	return 0;
}


int main(int argc, char *argv[])
{
	if ( argc <= 2 )
	{
		fprintf( stderr, "Error, incorrect usage.\n./linearUnmixer MU_SPECTRA REF_SPECTRA\n");
		return 3;
	}
	//spectrum files labelled  /mnt/c/Users/jakei/'OneDrive - McGill University'/'Year 2'/1BIEN210/'Group Project'/SpectrumFile_00N.txt
	//intialize and allocate memory for variabels
	int numRefs = argc - 2;
	//char *filePath = "";
	char *filePath = "/mnt/c/Users/jakei/OneDrive - McGill University/Year 2/1BIEN210/Group Project/";
	gsl_vector *muVector = gsl_vector_alloc(READINGS);
	gsl_vector *fVector = gsl_vector_alloc(numRefs);
	gsl_vector *x = gsl_vector_alloc(numRefs);			//TODO RENAME x AND PROBECOMPVECTOR
	gsl_matrix *refMatrix = gsl_matrix_alloc(READINGS, numRefs);
	gsl_matrix *C = gsl_matrix_alloc( numRefs, numRefs );
	gsl_matrix *Cinverse = gsl_matrix_alloc( numRefs, numRefs );
	
	int s;
	gsl_permutation *p = gsl_permutation_alloc(numRefs);			//CHANGED ALLOC TO CALLOC
	
	//initailize the variables to 0
	gsl_vector_set_zero(muVector);
	gsl_vector_set_zero(fVector);
	gsl_vector_set_zero(x);
	gsl_matrix_set_zero(refMatrix);
	gsl_matrix_set_zero(C);

	gsl_permutation_init(p);			//INITIALISED THE PERMUTATION
	
	readFileVector( muVector, filePath, argv[1] ); //call function which reads in values of first arguement file into the vector mu
	readFileMatrix( refMatrix, filePath, argc, argv ); //obtain matrix of reference vals, (400, 2) in 400 READINGS w/ 2 probes
	//normalize values, vector is already normalised
	normalizeVector( muVector );
	
	//print vector to csv file for plotting
	FILE *csvfile = fopen( "/mnt/c/Users/jakei/OneDrive - McGill University/Year 2/1BIEN210/Group Project/muVector.csv", "wt" );
	gsl_vector_fprintf( csvfile, muVector, "%f" );
	fclose(csvfile);
	FILE *csvfile2 = fopen( "/mnt/c/Users/jakei/OneDrive - McGill University/Year 2/1BIEN210/Group Project/refVectors.txt", "wt" );
	gsl_matrix_fprintf( csvfile2, refMatrix, "%f" );
	fclose(csvfile2);


	//now we have variable muVector with vals and refMatrix with vals.	
	gsl_blas_dgemm( CblasTrans, CblasNoTrans, 1, refMatrix, refMatrix, 0, C);	//matrix C is now A^T A
	gsl_blas_dgemv( CblasTrans, 1, refMatrix, muVector, 0, fVector);	//fVector is 'b'
	gsl_linalg_LU_decomp( C, p, &s);
//	gsl_linalg_LU_solve( C, p, fVector, x );	//can replace with gsl_linalg_LU_svx and delete x vector from program

	gsl_linalg_LU_invert( C, p, Cinverse );		//TODO maybe implement the other way you can solve for basis using inverse on both sides, (A^T A )^-1 A^T A x = (a^t a)^-1 (a^t b) = x 
	gsl_blas_dgemv( CblasNoTrans, 1, Cinverse, fVector, 0, x);	
/*	
	gsl_vector_fprintf( stdout, muVector, "%f" );
	printf("***********************************************\n");
*/	
	gsl_vector_fprintf( stdout, x, "%f" );
	
	gsl_matrix_free(Cinverse);
	gsl_matrix_free(C);
	gsl_matrix_free(refMatrix);
	gsl_vector_free(muVector);
	gsl_vector_free(fVector);
	gsl_vector_free(x);	
	return 0;

}
