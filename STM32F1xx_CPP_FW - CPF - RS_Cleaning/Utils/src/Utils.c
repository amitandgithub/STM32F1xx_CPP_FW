

#include "Utils.h"

// reverses a string 'str' of length 'len'

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
 
 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}
// Converts a floating point number to string.
int ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * power(10.0, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
	return i + afterpoint + 1 ;
}


/* Function to calculate x raised to the power y */
int power(int x, unsigned int y)
{
    if (y == 0)
        return 1;
    else if (y%2 == 0)
        return power(x, y/2)*power(x, y/2);
    else
        return x*power(x, y/2)*power(x, y/2);
}

/* Extended version of power function that can work
 for float x and negative y*/
 
float power(float x, int y)
{
    float temp;
    if( y == 0)
       return 1;
    temp = power(x, y/2);       
    if (y%2 == 0)
        return temp*temp;
    else
    {
        if(y > 0)
            return x*temp*temp;
        else
            return (temp*temp)/x;
    }
}


void mem_cpy (void* dst, const void* src, uint32_t cnt) 
{
	uint8_t *d = (uint8_t*)dst;
	const uint8_t *s = (const uint8_t*)src;

#if _WORD_ACCESS == 1
	while (cnt >= sizeof (int)) {
		*(int*)d = *(int*)s;
		d += sizeof (int); s += sizeof (int);
		cnt -= sizeof (int);
	}
#endif
	while (cnt--)
		*d++ = *s++;
}

int mem_cmp (const void* dst, const void* src, uint32_t cnt) 
{
	const uint8_t *d = (const uint8_t *)dst, *s = (const uint8_t *)src;
	int r = 0;

	while (cnt-- && (r = *d++ - *s++) == 0) ;
	return r;
}