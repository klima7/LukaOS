#ifndef __MATH_H__
#define __MATH_H__

#define MIN(_X, _Y) ((_X) < (_Y) ? (_X) : (_Y))
#define MAX(_X, _Y) ((_X) > (_Y) ? (_X) : (_Y))

double pow(double val, unsigned int n);
unsigned long long ullpow(unsigned long long val, unsigned int n);
double fabs(double val);
int ceil(double val);

#endif