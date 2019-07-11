#include <stddef.h>
#include <stdint.h>
#include "math.h"

double pow(double val, unsigned int n)
{
    double result = 1;
    for(unsigned int i=0; i<n; i++)
        result*=val;
    return result;
}

unsigned long long ullpow(unsigned long long val, unsigned int n)
{
    unsigned long long result = 1;
    for(unsigned int i=0; i<n; i++)
        result*=val;
    return result;
}

double fabs(double val)
{
    return val>=0 ? val : -val;
}

int ceil(double val)
{
    if(val > 0)
    {
        int x = (int)val;
        if(val - x == 0) return x;
        else return x+1;
    }

    else
    {
        int x = (int)val;
        return x;
    }
    
}
