

#include<stdio.h>
#include<math.h>
#include<stddef.h>
#include"stdint.h"

extern void reverse(char *str, int len);
extern int intToStr(int x, char str[], int d);
extern int ftoa(float n, char *res, int afterpoint);
extern float power(float x, int y);
extern int power(int x, unsigned int y);
extern void mem_cpy (void* dst, const void* src, uint32_t cnt) ;
extern int mem_cmp (const void* dst, const void* src, uint32_t cnt) ;
extern char a2i(char ch, char** src,int base,int* nump);
extern int a2d(char ch);