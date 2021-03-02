#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#include "norm.h"
#include "constants.h"

int readFileVector( gsl_vector *vector, char *filePath, char *fileName )                //read file vals into vector
{
	char tmpFilepath[120] = "\0";
	char tmpReading[20];
	char *tmp2;
	double reading;                                 //WARNING this is similar to global var $readings, consider renaming
	
	strcat( tmpFilepath, filePath );                //TODO replace this and the first instance of strcat into strcpy, that should work
	strcat( tmpFilepath, fileName );        //combines the filepath and name into one searchable /path/file
	FILE *file = fopen( tmpFilepath, "rt");
	printf("%s\n", tmpFilepath);                    //TODO DELETE THIS
	if ( file == NULL ) {                   //check if file exists
		fprintf( stderr, "Error, unable to locate the Mu data file\n");
		exit(100);
	}
	
	for ( int i = 0; i < skip; i++)         //move past header of read files
	{
		fgets( tmpReading, 19, file );
	}
	
	for ( int i = 0; i < readings; i++ )
	{
		fgets( tmpReading, 19, file );
		
		tmp2 = strrchr( tmpReading, '\t' );
		tmp2 = tmp2 + 1;
		reading = atof(tmp2);
		gsl_vector_set( vector, i, reading);
	}
	fclose(file);
	return 0;
}

int readFileMatrix( gsl_matrix *matrix, char *filePath, int argc, char *argv[] )
{
	char *tmp2;
	double reading;
	for ( int j = 2; j < argc; j++ )
	{
		gsl_vector_view column;
		char tmpFilepath[120] = "\0";                           //TODO replace this and the first instance of strcat into strcpy, that should work
		char tmpReading[20];
		strcat( tmpFilepath, filePath );
		strcat( tmpFilepath, argv[j] );         //combines the filepath and name into one searchable /path/file
		FILE *file = fopen( tmpFilepath, "rt");
		printf("%s\n", tmpFilepath);                            //TODO DELETE THIS
		if ( file == NULL ) {                   //check if file exists
			fprintf( stderr, "Error, unable to locate a reference data file\n");
			exit(100);
		}
		for ( int i = 0; i < skip; i++)         //move past header of read files
		{
			fgets( tmpReading, 20, file);
		}
		for ( int i = 0; i < readings; i++ ) //fill the ith element of the jth column
		{
			fgets( tmpReading, 19, file );
			
			tmp2 = strrchr( tmpReading, '\t' );
			tmp2 = tmp2 + 1;
			reading = atof(tmp2);
			gsl_matrix_set( matrix, i, j-2, reading );
		}
		fclose(file);
		column = gsl_matrix_column( matrix, j-2 );                              //NORMALISE EACH REFERENCE VECOTR.
		normalizeVector( &column.vector );
	}
	
	return 0;
}

