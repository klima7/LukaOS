#include <stddef.h>
#include <stdint.h>
#include "stdlib.h"
#include "string.h"

// Zamienia tekst na liczbe całkowitą ze znakiem - prosto z dante
int atoi(const char *str)
{
	int i=0, l=0, pow=1, len=(int)strlen(str);
	if(str[0]=='0' && len>1) return 0;
	for(i=0; i<(int)strlen(str); i++)
	{
		if(str[i]=='-') continue;
		if(str[i]<='9' && str[i]>='0') pow*=10;
		else break;
	}
	pow/=10;
	int sign=1;
	for(i=0; i<len; i++)
	{
		if(str[i]=='-')
		{
			if(i==0) { sign=-1; continue; }
			else return 0;
		}
		if(str[i] < '0' || str[i] > '9') return sign*l;
		l+=(str[i]-'0')*pow;
		pow/=10;
	}
	return sign*l;
}

// Zamienia tekst na liczbe całkowitą 64bit bez znaku
uint64_t atoul(const char *str)
{
	uint64_t l=0, pow=1;
	int len=(int)strlen(str), i=0;
	if(str[0]=='0' && len>1) return 0;
	for(i=0; i<(int)strlen(str); i++)
	{
		if(str[i]<='9' && str[i]>='0') pow*=10;
		else break;
	}
	pow/=10;
	for(i=0; i<len; i++)
	{
		if(str[i] < '0' || str[i] > '9') return l;
		l+=(str[i]-'0')*pow;
		pow/=10;
	}
	return l;
}

// Zamienia tekst na liczbe zmiennoprzecinkową - prosto z dante
float atof(const char *str)
{
	int index=0, count=0, i;
	const int len=(int)strlen(str);
	for(i=0; i<len; i++) 
	{
		if(str[i]=='.') { index=i; count++; }
		if(str[i]=='-' && i!=0) return 0;
	}
	if(count==0) index=len;
	if(count>1) return 0;
	const int INDEX = index;
	const unsigned int TEMP = len-INDEX+1;
	char a[INDEX+1], b[TEMP];
	for(i=0; i<index; i++) a[i]=str[i];
	for(i=0; i<(int)TEMP; i++) b[i]=0;
	for(i=0; i<len-index-1; i++) b[i]=str[index+1+i];
	a[index]=0; b[TEMP-1]=0;
	if(strlen(b)>5 && count==1) b[5]='\0'; 
	int p = (int)strlen(b);
	while(b[0]=='0') for(i=1; i<=len-index; i++) b[i-1]=b[i];
	if(len-index>6) b[6]=' ';
	int aa=atoi(a);
	int bb=atoi(b);
	if(count==0) bb=0;
	int dzielnik=1;
	for(i=0; i<p; i++) dzielnik*=10;
	return (float)aa+(float)bb/dzielnik*(str[0]=='-'?-1:1);
}

// Zamienia tekst zawiarający liczbe w postaci szesnastkowej na liczbe 32bit bez znaku
uint32_t xtou(const char *str)
{
	uint32_t l=0, pow=1;
	int len=(int)strlen(str), i=0;

	for(i=0; i<(int)strlen(str); i++)
	{
		if(!( (str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F') )) break;
		pow*=16;
	}

	pow/=16;

	for(i=0; i<len; i++)
	{
		uint32_t val = 0;
		if(str[i] >= '0' && str[i] <= '9') val = str[i] - '0';
		else if(str[i] >= 'a' && str[i] <= 'f') val = str[i] - 'a' + 10;
		else if(str[i] >= 'A' && str[i] <= 'F') val = str[i] - 'A' + 10;
		else return l;

		l+=val*pow;
		pow/=16;
	}
	return l;
}