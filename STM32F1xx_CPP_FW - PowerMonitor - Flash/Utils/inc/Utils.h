

#include<stdio.h>
#include<math.h>
#include<stddef.h>
#include"stdint.h"

 void reverse(char *str, int len);
 int intToStr(int x, char str[], int d);
int intToStr(int x, char str[], int d, char filler);
 int ftoa(float n, char *res, int afterpoint);
 int ftoa(float n, char *res, int afterpoint, char filler);
 float power(float x, int y);
 int power(int x, unsigned int y);
 void mem_cpy (void* dst, const void* src, uint32_t cnt) ;
 int mem_cmp (const void* dst, const void* src, uint32_t cnt) ;
 char a2i(char ch, char** src,int base,int* nump);
 int a2d(char ch);

 void Hex8ToStr( uint8_t byte, char* pBuffer );
 void Hex16ToStr( uint16_t word, char* pBuffer );
 void Hex32ToStr( uint32_t word, char* pBuffer);