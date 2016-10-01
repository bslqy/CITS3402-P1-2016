#include <stdlib.h>
#include <stdio.h>
#define MAX 1000000  //one row of double = sizeof(Double) *4400
#define LINE_LENGTH 15
#define ROW_SIZE 4400
#define COL_SIZE 500

char* readKey(char* fileName)
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


printf("%s\n",&keys[4399]);
  fclose(fp);
  return keys;

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

for(int i=0;i<row_size;i++)
{
    printf("%lf\n",col[i]);
}

return col;

}


/** use to return one col of the matrix after the col is scanned for the first time */
int* collection(double* one_column, int start,int row_size)
{
    // Put itself in a collection e.g. {1}
     int size_of_set = 1;
     int* collection_for_one_row = malloc(size_of_set*sizeof(int));
      collection_for_one_row[0] = start;


    for(int i=start;i<row_size-1;i++){
       // START is the benchmark of the dia checking operation
      //i.e. fixing START and find the DIA down the row
      // Put itself in a collection e.g. {START, START+1,START+2 ...}

    if(fabs(one_column[start]-one_column[i+1]) <= 0.5)  //If two row are in the same DIA
    {

        printf("%lf and %lf are in the dia\n" ,one_column[start],one_column[i+1]);

        //Put the row index in an array
        size_of_set++;

        collection_for_one_row= (int*)realloc(collection_for_one_row,size_of_set*sizeof(int)); //Resize the array, increase by one.
        collection_for_one_row[size_of_set-1] = i+1; // Add the index of the neighbour into the collection array.

    }


  }

  printf("size of set is %d\n",size_of_set);

     for(int i=0;i<size_of_set;i++)
  {
      printf("%d ",collection_for_one_row[i]);
      printf("\n");
  }

    return  collection_for_one_row;


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

 for(i=0;i<row;i++){//Read in the data
  for(j=0;j<col;j++){
   if (!fscanf(fp, "%lf", &jz[i][j]))
          {
            break;
          }
        }
 }

        /**Check the DIA*
         * Can be written in its own function
         **/

       /** return a col that given the col number*/
       /*  double* readCol(double** jz, int col_number,int row_size)   third argument can be removed */
       double* c = readCol(jz,0,3);

       /** return a set that contains the DIA neighbour while fixing a particular row*/
       /* int* collection(double* col, int start, int row_size) */
       int* one_collection =collection(c,0,3);



// for(i=0;i<col;i++){//Print the matrix
//    for(j=0;j<row-1;j++){
//       // printf("%lf first row, %lf second row\n absolute value of them is %lf\n ",jz[j][i],jz[j+1][i],fabs(jz[j][i]-jz[j+1][i]));
//
//        int number_of_neighbour = 1; // Count itself as a neighbour
//        int* collection_for_one_row = malloc(sizeof(int));
//
//
//       // j is the benchmark of the dia checking operation
//      //i.e. fixing j and find the DIA down the row
//        collection_for_one_row[0] = j;
//
//    if(fabs(jz[j][i]-jz[j+1][i]) < 0.4)  //If two row are in the same DIA
//    {
//        printf("%lf and %lf are in the dia\n" ,jz[j+1][i],jz[j][i]);
//        //Put the row index in an array
//        number_of_neighbour++;
//        realloc(collection_for_one_row,number_of_neighbour*sizeof(int)); //Resize the array, increase by one.
//        collection_for_one_row[number_of_neighbour] = j+1; // Add the index of the neighbour into the collection array.
//    }
//  }


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
