#include <stdlib.h>
#include <stdio.h>
#define MAX 1000000  //one row of double = sizeof(Double) *4400
#define LINE_LENGTH 15
#define ROW_SIZE 4400
#define COL_SIZE 500

void readKey(char* fileName)
{

char* keys[4400][LINE_LENGTH];

  int i = 0;
  FILE *fp;
  char *string=(char*)calloc(LINE_LENGTH,sizeof(char));

  fp = fopen(fileName, "r");

  while(1)
    {
      if(fgets(string, LINE_LENGTH, fp) == NULL)
         break;
         fgetc(fp);//Get rid of the space


      printf("\nline[%d] = %s\n", i+1, string);
      strcpy(keys[i],string);

      i++;
    }


printf("%s ",&keys[4399]);
  fclose(fp);

}

/** use to return one col of the matrix after the col is scanned for the first time */
double* readCol(double** jz, int col_number,int row_size)
{

printf("%d column is:\n",col_number);

double* col;
if((col = malloc(row_size* sizeof(double)))!=NULL) //The size should be 4400 for real
{
    for(int i=0;i<row_size;i++)
{
    col[i] = jz[i][col_number];
}
}

for(int i=0;i<3;i++)
{
    printf("%lf\n",col[i]);
}

}




void readMatrix(char* fileName)
{
	FILE *fp;
 int i,j;
 char s[MAX],ch;

 if((fp=fopen("3.txt","r"))==NULL)
 {
  printf("can not open this file!\n");
  exit(1);
 }
int row=0;
 while(fgets(s,MAX,fp)!=NULL)//Count the line
  row++;

 rewind(fp);//

int col=0;//Count the col
 while(ch!='\n')//(ch=fgetc(fp))!='\n'&&(ch=fgetc(fp))!='\r'
 {
  if(ch==' ')
   col++;
  ch=fgetc(fp);
 }
 col++;//

 double** jz= malloc(row*sizeof(double*));//Allocate spaces
 for(i=0;i<row;i++)
jz[i]=malloc(col*sizeof(double));

 rewind(fp);

 for(i=0;i<row;i++)//Read in the data
  for(j=0;j<col;j++){
   if (!fscanf(fp, "%lf", &jz[i][j]))
          {
            break;
          }
        }

        //Check the DIA
        readCol(jz,2,3);

 for(i=0;i<col;i++){//Print the matrix
    for(j=0;j<row-1;j++){
        printf("%lf first row, %lf second row\n absolute value of them is %lf\n ",jz[j][i],jz[j+1][i],fabs(jz[j][i]-jz[j+1][i]));


    if(fabs(jz[j][i]-jz[j+1][i]) < 0.4)
    {
        printf("%lf and %lf are in the dia\n" ,jz[j+1][i],jz[j][i]);
    }
  }
 }
 fclose(fp);

  printf("%d row, %d  col ",row,col);
    }




void swap(int a, int b)
{
    int temp;
    temp = a;
    a = b;
    b = temp;

}

void AllRange(int array[],int start , int len)
{
    if(start == len-1) //Base case
    {
        for(int j = 0; j<len;j++)
        {
            printf("%d ",array[j]);
        }
        printf("\n");
        return;
    }

        for(int i=start;i<len-1;i++)
        {
            swap(array[start],array[i]);
            AllRange(array,start + 1,len);
            swap(array[start],array[i]);
        }


}




int main(void) {

//readKey("keys.txt");
//readMatrix("3.txt");

readMatrix("2.txt");

}
