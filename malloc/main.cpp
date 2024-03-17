#include"malloc.h"
#include"malloc.cpp"
int main()
{
int *a =(int*)malloc_(30);
int *b =(int*)malloc_(1);
int *c =(int*)malloc_(10000);
memset(a,0,30);
memset(b,0,1);
memset(c,0,10000);
free_(a);
free_(b);
free_(c);
a =(int*)malloc_(327);
b =(int*)malloc_(1354);
c =(int*)malloc_(106117);
free_(a);
free_(b);
free_(c);
return 0;
}