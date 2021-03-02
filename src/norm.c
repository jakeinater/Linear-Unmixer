#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

int normalizeVector( gsl_vector *vector )
{
	double min = gsl_vector_min( vector );
	double norm = gsl_vector_max( vector ) - min;
	gsl_vector_add_constant( vector, -min );
	gsl_vector_scale( vector, 1/norm );
	return 0;
}
