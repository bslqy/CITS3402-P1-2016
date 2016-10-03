#include <stdio.h>
#include <stdlib.h>

char array[] = "abcd";

#define N 4
#define M 3
int queue[N] = {0};
int top = 0;
int flag[N] = {0};
 
 void perm(int s, int n)
 {
   int i;
 
   if (s > n)
   {
     return;
   }
 
   if (s == n)
   {
     for (i = 0; i < n; i++)
     {
       printf("%c", queue[i]);
     }
     printf("\t");
     return ;
   }
 
   for (i = 0; i < n; i++)
   {
     if (flag[i] == 0)
     {
       flag[i] = 1;
       queue[s] = array[i];
       perm(s+1, n);
       flag[i] = 0;
     }
   }
 }
 
 void comb(int s, int n, int m)
 {
   int i;
 
   if (s > n)
     return ;
 
   if (top == m)
   {
     for (i = 0; i < m; i++)
     {
       printf("%c", queue[i]);
     }
     printf("\t");
     return ;
   }
 
   queue[top++] = array[s];
   comb(s+1, n, m);
   top--;
   comb(s+1, n, m);
 
 }
 
 int main()
 {
   printf("\nperm():\n");
   perm(0, N);
   printf("\ncombination():\n");
   comb(0, N, M);
   printf("\n");
   return 0;
 }
