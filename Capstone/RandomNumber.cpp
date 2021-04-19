#include "RandomNumber.h"

RandomNumber* RandomNumber::pInstance = NULL;

RandomNumber::RandomNumber(void)
{
	seed = (unsigned)time(NULL);
	ran1idnum = -(long)seed;
	qd2idnum = -(int)seed;
	srand(seed);
}
RandomNumber::~RandomNumber(void){}

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)
double  RandomNumber::ran1(){

	int j;
	long k;
	static long iy=0;
	static long iv[NTAB];
	float temp;
	if (ran1idnum <= 0 || !iy) {
		if (-(ran1idnum) < 1) ran1idnum=1;
		else ran1idnum = -(ran1idnum);
		for (j=NTAB+7;j>=0;j--) {
			k=(ran1idnum)/IQ;
			ran1idnum=IA*(ran1idnum-k*IQ)-IR*k;
			if (ran1idnum < 0) ran1idnum += IM;
			if (j < NTAB) iv[j] = ran1idnum;
		}
		iy=iv[0];
	}
	k=(ran1idnum)/IQ;
	ran1idnum=IA*(ran1idnum-k*IQ)-IR*k;
	if (ran1idnum < 0) ran1idnum += IM;
	j=iy/NDIV;
	iy=iv[j];
	iv[j] = ran1idnum;
	temp=(float)AM*iy;
	if ( temp > RNMX){ 
		return RNMX;
	}else{
		return temp;
	}
}
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX