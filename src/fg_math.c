#ifndef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include "common.h"
#include "debug.h"
#include "fg_math.h"

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#ifdef HAVE_LIBGSL 
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#endif

#ifndef HAVE_LIBGSL

#ifdef __SOLARIS__
#define RANDOM_MAX              4294967295UL    /* 2**32-1 */
#elif __DARWIN__
#define RANDOM_MAX              LONG_MAX        /* Darwin */
#else
#define RANDOM_MAX              RAND_MAX        /* Linux, FreeBSD */
#endif

#endif

#ifdef HAVE_LIBGSL
const gsl_rng_type * T;
gsl_rng * r;
#endif

extern void
rn_set_seed (int i) {
#ifdef HAVE_LIBGSL
	gsl_rng_env_setup();
        T = gsl_rng_default;
        r = gsl_rng_alloc (T);
	gsl_rng_set (r, i);
	DEBUG_MSG(LOG_WARNING, "initalized libgsl random functions with seed %u",(unsigned)i);
#else
	srand((unsigned)i);
	DEBUG_MSG(LOG_WARNING, "initalized posix random functions with seed %u",(unsigned)i);
#endif
}

extern int
rn_read_dev_random () {
	int i, rc;
	int data = open("/dev/urandom", O_RDONLY);
	rc = read(data, &i, sizeof (int) );
	close(data);
	if(rc == -1) {
		error(ERR_FATAL, "read /dev/urandom failed: %s", strerror(errno));
	}
	return i;
}

static inline double
rn_uniform() { 
#ifdef HAVE_LIBGSL
	return gsl_rng_get(r);
#else
	return (rand());
#endif
}

static inline double
rn_uniform_zero_to_one() { 
#ifdef HAVE_LIBGSL
	return gsl_rng_uniform_pos(r);
#else
	return (rn_uniform()/(RANDOM_MAX+1.0) );
#endif
}

static inline double
rn_uniform_minusone_to_one() { return (rn_uniform()/(RANDOM_MAX/2.0)-1.0); }

double
dist_exponential(const double mu) { 
#ifdef HAVE_LIBGSL
	return gsl_ran_exponential(r, mu);	
#else
	return (-log(rn_uniform())+mu);
#endif
}

/* source english wikipedia articles */

double
dist_uniform(const double minval, const double maxval) {
	const double x = rn_uniform_zero_to_one();
	return ((maxval-minval) * x) + minval;
}
double
dist_normal(const double mu, const double sigma_square) {
#ifdef HAVE_LIBGSL
	return gsl_ran_gaussian (r, sigma_square) + mu;
#else
	const double x = rn_uniform_minusone_to_one();
	return ( 1.0 / sqrt(2.0*M_PI*sigma_square) ) * exp( (-pow ((x-mu),2) ) / ( 2 * sigma_square) );
#endif
}

extern int
dist_bernoulli(const double p) { 
#ifdef HAVE_LIBGSL
	return gsl_ran_bernoulli (r, p);
#else
	return (rn_uniform_zero_to_one() <= p); 
#endif
}

extern double
dist_pareto (const double k, const double x_min) {
#ifdef HAVE_LIBGSL
	return gsl_ran_pareto (r, k, x_min);
#else
        const double x = rn_uniform();
        if (x < x_min) return 0;
        else return ( (k/x_min) * pow (x_min/rn_uniform(),k+1) );
#endif
}

extern double
dist_weibull (const double alpha, const double beta) {
#ifdef HAVE_LIBGSL
	return gsl_ran_weibull(r, alpha, beta);
#else
        const double x = rn_uniform_zero_to_one();
        return  alpha * beta * pow (x,beta-1.0) * exp( -alpha * pow(x,beta) );
#endif
}

extern double
dist_chisq (const double nu) {
#ifdef HAVE_LIBGSL
	return gsl_ran_chisq(r, nu);
#else
	UNUSED_ARGUMENT(nu);
	return 0;
#endif	
}
