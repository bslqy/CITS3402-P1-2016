#include <stdlib.h>
#include <stdio.h>
#define MAX 1000000  //one row of double = sizeof(Double) *4400
#define LINE_LENGTH 15
#define ROW_SIZE 4400
#define COL_SIZE 500


/** individual block **/
struct block
{
    int row1;
    int row2;
    int row3;
    int row4;
    char* signature;
};

/** A collection of blocks for one column **/
struct Blocks
{
    struct block;
    int COL_INDEX;
};


char* readKey(char* fileName)
{

char keys[4400][LINE_LENGTH];

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


  fclose(fp);
  return keys;

}

char* getKey()
{

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



/** use to return a collection of DIA neighbours given a fixed row (start)  */
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

 int** combination(int* array,int n, int m)
 {


 }

 /** Function to add two signature together */

 char* Add_Two_Num(char* num1, char* num2){


    int len1,len2;
    int carry = 0;
    int i,j,k;
    int temp;

    len1 = strlen(num1);
    len2 = strlen(num2);
    char* result;
    k = 0;

    // (Maximum_size) N*4  is one more than the size_of_N
    if((result=malloc(len1*sizeof(char)))==NULL)
    {
        exit(1);
    }


    //Add the number from the lowest place   i.e. from right to left.
    for(i=len1-1,j=len2-1;i>=0 && j>=0;i--,j--){
        temp = num1[i]-'0'+num2[j]-'0'+carry; //Convert a char to int.
        if(temp>=10){
            temp -= 10;
            carry = 1;
        }else{
            carry = 0;
        }

        result[k] = temp+'0';       //store the results as strings
        k++;
    }

    //Finishing the rest of the addition, assuming two integer are of difference size.
    while(i>=0){
        temp = num1[i]-'0'+carry;
        if(temp>=10){
            temp -= 10;
            carry = 1;
        }else{
            carry = 0;
        }

        result[k] = temp+'0';
        i--;
        k++;
    }

    while(j>=0){
        temp = num2[j]-'0'+carry;
        if(temp>=10){
            temp -= 10;
            carry = 1;
        }else{
            carry = 0;
        }
        result[k] = temp+'0';
        j--;
        k++;
    }

    if(carry){      //if there is still a carry for the addition of two number at the head
        result[k] = 1+'0';
        result = (char*)realloc(result,(len1+1)*sizeof(char));
        }
    else{           //move the index back to the last place of the result array
        k--;
    }

    while(k>=0){   //print the result reversely
        printf("%c ",result[k]);
        printf("size = %d\n",k);
        k--;
    }
    printf("\n");
    printf("length of result = %d\n",strlen(result));
    return result;
}

struct block Create_Block(int* arr)
{
    struct block b;
    b.row1 = arr[0];
    b.row2 = arr[1];
    b.row3 = arr[2];
    b.row4 = arr[3];
//    b.signature = malloc(strlen(Add_Two_Num(Add_Two_Num(keys[b.row1],keys[b.row2]),Add_Two_Num(keys[b.row1],keys[b.row2]))));
  //  strcpy(b.signature,Add_Two_Num(Add_Two_Num(keys[b.row1],keys[b.row2]),Add_Two_Num(keys[b.row1],keys[b.row2])));

    return b;
};




void readMatrix(char* fileName)
{
 FILE *fp;
 int i,j;
 char s[MAX],ch;

 if((fp=fopen("fileName","r"))==NULL)
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


 fclose(fp);

  printf("%d row, %d  col ",row,col);
}

int main(void) {

//readKey("keys.txt");
//readMatrix("3.txt");

/** store the keys **/
char* keys = readKey("keys.txt");
//int* arr = {1,2,3,4};
//struct block b = Create_Block(arr);
//printf("%c",b.signature[0]);
for(int i = 0;i<15;i++){
    printf("%c,",keys[4355*15+i]);
}

//readMatrix("2.txt");
}
