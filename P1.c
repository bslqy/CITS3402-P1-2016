#include <stdlib.h>
#include <stdio.h>
#define MAX 1000000  //one row of double = sizeof(Double) *4400
#define LINE_LENGTH 15
#define ROW_SIZE 4400
#define COL_SIZE 500
#define DIA 0.000001
#define M  4        // SIZE OF COMBINATION



/** individual block **/
struct block
{
    int col_index;
    int row1;
    int row2;
    int row3;
    int row4;
    char signature[15];
};


char* KEYS;                 //  GLOBAL KEY ARRAY

    //  GLOBAL VARIAABLES USED FOR BLOCK CREATION
struct block b;             //  RECYCLCABLE SINGLE BLOCK
struct block* Big_Block;    //  COLLECTION OF ALL BLOCK
int NUM_OF_BLOCK = 0;       //  TOTAL NUMBER OF BLOCK

    //  GLOBAL VARIABLE USED FOR COMBINATION
int SIZE_OF_COLLECTION=0;     // SIZE OF SET FOR COMBINATION CALCULATION N choose M
int queue[4400] = {0} ;           // THE TEMPERORY ARRAY THAT STORE THE INSTANCE OF THE CONBINATION
int result[M] = {0};        // THE ARRAY THAT SAVE THE RESULT OF ONE COMBINATION
int top = 0;                // THE POINTER OF RECURSION




char* readKey(char* fileName)
{

char keys[4550][LINE_LENGTH]; // 4400 ROWS SIZE HAS OVERFLOW PROBLEM

  int i = 0;
  FILE *fp;
  char *string=(char*)calloc(LINE_LENGTH,sizeof(char));

  fp = fopen(fileName, "r");

  while(1)
    {
      if(fgets(string, LINE_LENGTH, fp) == NULL)
         break;
         fgetc(fp);         //GET RID OF THE WHITE SPACE


     // printf("\nline[%d] = %s\n", i+1, string);
      strcpy(keys[i],string);

      i++;
    }


  fclose(fp);
  return keys;

}

    /** FUNCTION THAT OBTAIN THE CORRESPOINDING KEY GIVEN A ROW NUMBER **/

char* getKey(int row_index)
{
    char* temp = malloc(LINE_LENGTH*sizeof(char));
    for(int i = 0;i<LINE_LENGTH;i++){
        temp[i] = KEYS[LINE_LENGTH*row_index+i];
    }

    return temp;

}




/** use to return one col of the matrix after the col is scanned for the first time */
double* readCol(double** jz, int col_number,int row_size)
{

	//printf("%d column is:\n",col_number);

double* col;
if((col = malloc(row_size* sizeof(double)))!=NULL)
{
    for(int i=0;i<row_size;i++)
{
    col[i] = jz[i][col_number];
}
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

    if(fabs(one_column[start]-one_column[i+1]) <= DIA)  //If two row are in the same DIA
    {

        printf("%lf and %lf are in the dia\n" ,one_column[start],one_column[i+1]);

        //Put the row index in an array
        size_of_set++;
        collection_for_one_row= (int*)realloc(collection_for_one_row,size_of_set*sizeof(int)); //Resize the array, increase by one.
        collection_for_one_row[size_of_set-1] = i+1; // Add the index of the neighbour into the collection array.
    }
  }
  SIZE_OF_COLLECTION = size_of_set;  // MODIFY THE ARRAY SIZE N FOR COMBINATION.


 // printf("size of set is %d\n",size_of_set);


//    if(size_of_set<4){
//        return ;
//    }
//    else{
//     printf("size of set is %d\n",size_of_set);
//      for(int i=0;i<size_of_set;i++)
//    {
//      printf("%d ",collection_for_one_row[i]);
//      printf("\n");
//    }
//  }

    //  ONLY INTERESTED IN THE SET WITH SIZE>4

     printf("size of set is %d\n",size_of_set);
      for(int i=0;i<size_of_set;i++)
    {
      printf("%d ",collection_for_one_row[i]);
      printf("\n");
    }
    return  collection_for_one_row;


 }



 void comb(int* input,int s, int n, int m)
 {
     int i;

     if (s > n)
         return ;

     if (top == m)
     {

         for (i = 0; i <m; i++)
         {
             result[i] = queue[i];   // SAVE THE RESULT TO THE GLOBAL ARRAY FOR ONE PARTICULAR INSTANCE OF THE COMBINATION
             //printf("%c",result[i]);
             printf("%d\n", queue[i]);
         }

         for(int i=0;i<m;i++)
        printf("This is result:  %d\n",result[i]);

     }

     queue[top++] = input[s];
     comb(input,s+1, n, M);
     top--;
     comb(input,s+1, n, M);

 }


/** REVSER THE RESULT STRING FOR Add_Two_Num() FUNCTION **/

char *revstr(char *str, size_t len)
{

    char    *start = str;
    char    *end = str + len - 1;
    char    ch;

    if (str != NULL)
    {
        while (start < end)
        {
            ch = *start;
            *start++ = *end;
            *end-- = ch;
        }
    }
    return str;
}



 /** FUNCTION TO ADD TWO SIGNATURE TOGETHER */

 char* Add_Two_Num(char* num1, char* num2){

    char* result;
    int len1,len2;
    int carry = 0;
    int i,j,k;
    int temp;

    len1 = strlen(num1);
    len2 = strlen(num2);
    k = 0;

    // (Maximum_size) N*4  is one more than the size_of_N
    if((result=malloc((len1+1)*sizeof(char)))==NULL)
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
        result = (char*)realloc(result,(len1+2)*sizeof(char));
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

    //REVESRE THE RESULT ARRAY;
    revstr(result,strlen(result));
    return result;
}


double** readMatrix(char* fileName){

	FILE *fp;
	int i,j;
	char s[MAX],ch;

	if((fp = fopen(fileName,"r")) == NULL){
		printf("can not open this file!\n");
		exit(1);
 	}

	int row=0;

	 while(fgets(s,MAX,fp)!=NULL)//Count the line
		 row++;

	 rewind(fp);//

	 int col=0;//Count the col

	 while(ch != '\n')//(ch=fgetc(fp))!='\n'&&(ch=fgetc(fp))!='\r'
	 {
		 if(ch == ',')
			 col++;
		 ch=fgetc(fp);

		 //printf("this is the m %d \n", col);


	 }

	 col++;//

	 //printf("instance: %d  col \n",col);

 	double** jz= malloc(row*sizeof(double*));//Allocate spaces

	for(i=0;i<row;i++){
		jz[i]=malloc(col*sizeof(double));
	}

	 rewind(fp);

	 for(i=0;i<row;i++){//Read in the data
		 for(j=0;j<col;j++){

			 if (!fscanf(fp, "%lf%*[,]", &jz[i][j]))
			 {

					 break;
			 }

			 //printf("isthisit? %lf, row: %d, col: %d \n, rowlim: %d, collim: %d", jz[i][j], i, j, row, col);
		 }
	 }

 fclose(fp);

 printf("%d row, %d  col \n",row,col);

 return jz;
}



/** RECYCLING THE GLOBAL STRUCT B **/
void Create_Block(int arr[],int COL_INDEX)
{
    b.col_index = COL_INDEX;
    b.row1 = arr[0];
    b.row2 = arr[1];
    b.row3 = arr[2];
    b.row4 = arr[3];
    strcpy(b.signature, Add_Two_Num(Add_Two_Num(getKey(b.row1),getKey(b.row2)),Add_Two_Num(getKey(b.row3),getKey(b.row4))));

}


/** RECYCLING THE GLOBAL STRUCT B **/

void Add_To_Block_Collection()
{
    //FIRST ALLOCATION
    if(NUM_OF_BLOCK == 0){
        if((Big_Block =(malloc(sizeof(struct block))))== NULL)
        {
            printf("Memory Allocation fails\n");
            exit(1);
        }
        Big_Block[NUM_OF_BLOCK] = b;// STORE THE INSTANCE OF BLOCK INTO THE COLLECTION , b IS THE RECYCLABLE BLOCK WHICH GETS FREED EVERYTIME
        NUM_OF_BLOCK++;

    }

    else{
        //SEQUENTIAL ACCESS
        NUM_OF_BLOCK++; //ADD ONE MORE BLOCK TO THE COLLECTION OF BLOCKS
        Big_Block = realloc(Big_Block,NUM_OF_BLOCK * sizeof(struct block));
        Big_Block[NUM_OF_BLOCK-1] = b; // STORE THE INSTANCE OF BLOCK INTO THE COLLECTION, b IS THE RECYCLABLE BLOCK WHICH GETS FREED EVERYTIME
    }

        printf("This is big block 1 signature %s\n",Big_Block[NUM_OF_BLOCK-1].signature);

}

///** FINAL OUTPUT **/
//void Collision(int start)
//{
//    //  FIXING THE ROW AND COMPARE THE SIGNATURE DOWNWARD, SAME AS FINDING DIA NEIGHBOUR
//    for(int i = start;i<NUM_OF_BLOCK-1;i++)
//    {
//
//            if((strcmp(Big_Block[i].signature,Big_Block[i+1].signature)==0) && (Big_Block[i].col_index != Big_Block[i+1].col_index))
//            {
//
//                printf("Block %d and Block %d collide\n",i,i+1);
//                printf("Row Index From Block %d: %d %d %d %d \n",i,Big_Block[i].row1,Big_Block[i].row2,Big_Block[i].row3,Big_Block[i].row4);
//                printf("Row Index From Block %d: %d %d %d %d \n",i+1,Big_Block[i+1].row1,Big_Block[i+1].row2,Big_Block[i+1].row3,Big_Block[i+1].row4);
//
//            }
//
//    }
// printf("HHAHA");
//
//
//}

/** FINAL OUTPUT **/
void Collision()
{
    //  FIXING THE ROW AND COMPARE THE SIGNATURE DOWNWARD, SAME AS FINDING DIA NEIGHBOUR
    for(int i = 0;i<NUM_OF_BLOCK-1;i++)
    {
        for(int j = i+1;j<=NUM_OF_BLOCK-1;j++){
               // printf("j  is %d\n",j);
               // printf("i  is %d\n",i);
            if((strcmp(Big_Block[i].signature,Big_Block[j].signature)==0) && (Big_Block[i].col_index != Big_Block[j].col_index))
            {
                printf("Block %d and Block %d collide\n",i,j);
                printf("Row Index From Block %d: %d %d %d %d \n",i,Big_Block[i].row1,Big_Block[i].row2,Big_Block[i].row3,Big_Block[i].row4);
                printf("Row Index From Block %d: %d %d %d %d \n",j,Big_Block[j].row1,Big_Block[j].row2,Big_Block[j].row3,Big_Block[j].row4);

            }
            else{
                printf("No Collision Happens for Block %d and Block %d \n",i,j);
            }
        }

    }



}



/******** ======== *********/

int main(void) {

	//readMatrix("data.txt");

/** store the keys **/

   // KEYS = readKey("keys.txt");
	double** jz = readMatrix("data.txt");


//printf("Finish!!!!!!!!\n",getKey(1));
//	int arr1[4] = {4399,4398,4397,4396};
//	int arr2[4] = {1,2,3,4};
//    Create_Block(arr1,1);
//    Add_To_Block_Collection();
//
//    printf("COL_INDEX : %s\n",Big_Block[0].signature);
//
//    Create_Block(arr1,2);
//    Add_To_Block_Collection();
//    printf("COL_INDEX : %s\n",Big_Block[1].signature);
//
//    Create_Block(arr1,2);
//    Add_To_Block_Collection();
//    printf("COL_INDEX : %s\n",Big_Block[2].signature);
//
//
//    Collision();
//    free(Big_Block); //IN THE END

    printf("Size Before one_collection is %d\n",SIZE_OF_COLLECTION);

    double*c  = readCol(jz,499,4400);
    int* one_collection = collection(c,0,4400);
    printf("Size from one_collection is %d\n",SIZE_OF_COLLECTION);


    printf("Change the size of collection to 5 \n",SIZE_OF_COLLECTION);

    SIZE_OF_COLLECTION = 5;

    printf("After changing the size is %d  \n",SIZE_OF_COLLECTION);

     int array[] = {1,2,3,4,5};
     printf("\ncombination():\n");
     comb(one_collection,0,SIZE_OF_COLLECTION,M);

     printf("Size After Combination is %d\n",SIZE_OF_COLLECTION);
     printf("Finish!");


}
