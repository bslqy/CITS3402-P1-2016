/*
* Student Number1: 21448107
* Student Name1: Cikang Li
*
* Student Number2: 21480279
* Student Name2: Jia Liao
*/



#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
//#include <omp.h>

#define MAX 1000000  //one row of double = sizeof(Double) *4400
#define ROW_SIZE 4400
#define COL_SIZE 500
#define DIA 0.000001
#define M  4        // SIZE OF COMBINATION

/** individual block **/
struct block {
	int col_index;
	int row1;
	int row2;
	int row3;
	int row4;
	long long signature;
};

long long* KEYS;   //GLOBAL KEY STORAGE


				   //  GLOBAL VARIAABLES USED FOR BLOCK CREATION
struct block b;             //  RECYCLCABLE SINGLE BLOCK
struct block* Big_Block;    //  COLLECTION OF ALL BLOCK
int NUM_OF_BLOCK = 0;       //  TOTAL NUMBER OF BLOCK

							//  GLOBAL VARIABLE USED FOR COMBINATION
int SIZE_OF_COLLECTION = 0;     // SIZE OF SET FOR COMBINATION CALCULATION N choose M
								//int SIZE_OF_COMBINATION = 1;    // SIZE OF COMBINATION nCm

void readKey(char* fileName) {

	KEYS = malloc(4400 * sizeof(long long));

	int i = 0;
	FILE *fp;

	fp = fopen(fileName, "r");

	while (i<4400) {
		if (!fscanf(fp, "%lli", &KEYS[i])) {
			break;
		}

		fgetc(fp);         //GET RID OF THE WHITE SPACE

		i++;
	}

	fclose(fp);

}

/** FUNCTION THAT OBTAIN THE CORRESPOINDING KEY GIVEN A ROW NUMBER **/

long long getKey(int row_index) {
	return KEYS[row_index];
}




/** use to return one col of the matrix after the col is scanned for the first time */
double* readCol(double** jz, int col_number, int row_size) {

	double* col;

	if ((col = malloc(row_size * sizeof(double))) != NULL) {
		for (int i = 0; i < row_size; i++) {
			col[i] = jz[i][col_number];
		}
	}

	return col;
}


/** use to return a collection of DIA neighbours given a fixed row (start)  */
int* collection(double* one_column, int start, int row_size) {
	// Put itself in a collection e.g. {1}
	int size_of_set = 1;
	int* collection_for_one_row = malloc(size_of_set * sizeof(int));

	collection_for_one_row[0] = start;

	int i =start;
//#pragma omp parallel for firstprivate(i) shared(collection_for_one_row) 
	for (i; i < row_size - 1; i++) {
		// START is the benchmark of the dia checking operation
		//i.e. fixing START and find the DIA down the row
		// Put itself in a collection e.g. {START, START+1,START+2 ...}

		if (fabs(one_column[start] - one_column[i + 1]) <= DIA) {    //If two row are in the same DIA
																	 //Put the row index in an array
			size_of_set++;
			collection_for_one_row = (int*)realloc(collection_for_one_row, size_of_set * sizeof(int)); //Resize the array, increase by one.
			collection_for_one_row[size_of_set - 1] = i + 1; // Add the index of the neighbour into the collection array.
		}
	}

	SIZE_OF_COLLECTION = size_of_set;  // MODIFY THE ARRAY SIZE N FOR COMBINATION.
									   // printf("Inside the collection function, what is the SIZE_OF_COLLECTION? %d\n",SIZE_OF_COLLECTION);


									   /*
									   if(size_of_set > 4){
									   for(int i=0;i<size_of_set;i++){
									   printf("%d ",collection_for_one_row[i]);
									   }
									   printf("\n");

									   }

									   */
	return collection_for_one_row;
}



/** modify the size of nCm for different input set **/
/** nCm = n!/m!(n-m)! **/
/** Use natural log to reduce the size **/

int get_combination_size(int n, int m) {
	return  floor(round(exp(lgamma(n + 1) - lgamma(n - m + 1)) / tgamma(m + 1)));
}


int** combNonRec(int collect[], int sizeOfColl, int sizeOut) {


	int size_of_combination = get_combination_size(sizeOfColl, sizeOut);
	int index, i, *p;
	int** finalR = (int**)malloc(sizeof(int*) * size_of_combination);


	for (int i = 0; i < size_of_combination; i++) {
		finalR[i] = (int*)malloc(sizeof(int) * 4);
	}

	p = (int*)malloc(sizeof(int) * sizeOut);

	int thisInd = 0;
	index = 0;

	p[index] = 0;//取第一个元素

	while (true) {
		if (p[index] >= sizeOfColl) {//取到底了，回退
			if (index == 0) {//各种情况取完了，不能再回退了
				break;
			}

			index--;//回退到前一个
			p[index]++;//替换元素
		}
		else if (index == sizeOut - 1) {//取够了，输出
			for (i = 0; i < sizeOut; i++) {
				finalR[thisInd][i] = collect[p[i]];
			}

			thisInd++;
			p[index]++; //替换元素
		}
		else {//多取一个元素
			index++;
			p[index] = p[index - 1] + 1;
		}

	}

	free(p);

/*
	for (int i = 0; i<size_of_combination; i++)
	{
		printf("List %d:\n", i);
		for (int j = 0; j<4; j++)
		{
			printf("%d ", finalR[i][j]);
		}
		printf("\n");
		printf("\n");
	}

	*/
	return finalR;
}



/** RECYCLING THE GLOBAL STRUCT B **/
void create_Block(int arr[], int COL_INDEX) {

	b.col_index = COL_INDEX;
	b.row1 = arr[0];
	b.row2 = arr[1];
	b.row3 = arr[2];
	b.row4 = arr[3];
	b.signature = getKey(arr[0]) + getKey(arr[1]) + getKey(arr[2]) + getKey(arr[3]);
}




/** RECYCLING THE GLOBAL STRUCT B **/
void add_To_Block_Collection() {
	//FIRST ALLOCATION
	if (NUM_OF_BLOCK == 0) {
		if ((Big_Block = (malloc(sizeof(struct block)))) == NULL) {
			printf("Memory Allocation fails\n");
			exit(1);
		}

		Big_Block[NUM_OF_BLOCK] = b;// STORE THE INSTANCE OF BLOCK INTO THE COLLECTION , b IS THE RECYCLABLE BLOCK WHICH GETS FREED EVERYTIME
		NUM_OF_BLOCK++;
	}

	else {
		//SEQUENTIAL ACCESS
		NUM_OF_BLOCK++; //ADD ONE MORE BLOCK TO THE COLLECTION OF BLOCKS
		Big_Block = realloc(Big_Block, NUM_OF_BLOCK * sizeof(struct block));
		Big_Block[NUM_OF_BLOCK - 1] = b; // STORE THE INSTANCE OF BLOCK INTO THE COLLECTION, b IS THE RECYCLABLE BLOCK WHICH GETS FREED EVERYTIME
	}
}


/** FINAL OUTPUT **/
void collision() {
	//  FIXING THE ROW AND COMPARE THE SIGNATURE DOWNWARD, SAME AS FINDING DIA NEIGHBOUR
	int i = 0;
	int j = i + 1;
#pragma omp parallel for firstprivate(i,j) shared(NUM_OF_BLOCK)
	for (  i =0  ; i < NUM_OF_BLOCK - 1; i++) {
		for (  j = i+1; j <= NUM_OF_BLOCK - 1; j++) {
			if ((Big_Block[i].signature == Big_Block[j].signature) && (Big_Block[i].col_index != Big_Block[j].col_index)) {
				printf("Block %d from Col %d and Block %d from Col %d collide\n", i, Big_Block[i].col_index, j, Big_Block[j].col_index);
				printf("Row Index From Block %d: %d %d %d %d \n", i, Big_Block[i].row1, Big_Block[i].row2, Big_Block[i].row3, Big_Block[i].row4);
				printf("Row Index From Block %d: %d %d %d %d \n", j, Big_Block[j].row1, Big_Block[j].row2, Big_Block[j].row3, Big_Block[j].row4);

			}
			else {
				// printf("No collision Happens for Block %d and Block %d \n",i,j);
			}
		}
	}
}


double** readMatrix(char* fileName) {

	FILE *fp;
	int i, j;
	char s[MAX], ch="";

	if ((fp = fopen(fileName, "r")) == NULL) {
		printf("can not open this file!\n");
		exit(1);
	}

	int row = 0;

	while (fgets(s, MAX, fp) != NULL) {//Count the line
		row++;
	}

	rewind(fp);//

	int col = 0;//Count the col

	while (ch != '\n') {
		if (ch == ',') {
			col++;
		}

		ch = fgetc(fp);
	}

	col++;

	double** jz = malloc(row * sizeof(double*));//Allocate spaces

	for (i = 0; i < row; i++) {
		jz[i] = malloc(col * sizeof(double));
	}

	rewind(fp);

	for (i = 0; i < row; i++) {//Read in the data
		for (j = 0; j < col; j++) {
			if (!fscanf(fp, "%lf%*[,]", &jz[i][j])) {
				break;
			}
		}
	}

	fclose(fp);

	printf("%d row, %d  col \n", row, col);

	return jz;
}



/******** ======== *********/


int main(void) {

	

	clock_t begin = clock();

	readKey("keys.txt");
	double** jz = readMatrix("data.txt");

	int i = 0;
	int j = 0;
	#pragma omp parallel for lastprivate(NUM_OF_BLOCK) 
		for (i = 0; i < 499; i++) {  

		printf("THIS IS COLUMN %d\n", i);

		double*c = readCol(jz, i, 4400);
		//int j = 0;

		//#pragma omp parallel for firstprivate(j) shared(c)
		for ( j = 0; j < 4400; j++) {

			// printf("This is fixed row %d !!!!!!!!!!\n",j);

			 int* one_collection = collection(c, j, 4400);


			// MODIFY THE DYMANIC ALLOCATION OF SPACES (SIZE_OF_COMBINATION) IN combNonRec() function.

			 
				 if (get_combination_size(SIZE_OF_COLLECTION, M) >= 4) {
					 //GET THE 2D-ARRAY OF COMBINATION

					 int** rrr = combNonRec(one_collection, SIZE_OF_COLLECTION, M);

					 int ii = 0;
					 //#pragma omp parallel for private(ii)
					 for (ii = 0; ii < get_combination_size(SIZE_OF_COLLECTION, M); ii++) {

						 create_Block(rrr[ii], i);  // first arg: int[] ; second arg: col_number;
						 add_To_Block_Collection();
						 // printf("\n");
					 }

					 free(rrr);


				 }

				 free(one_collection);

			 
		}

		//OpenMP for j



		free(c);
	}
		
	// OpenMP for i
	collision();
	

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Total time spent : %f\n", time_spent);

	printf("NUMBER OF BLOCK  is %d", NUM_OF_BLOCK);


	

	/*
	int i = 0;
#pragma omp parallel for lastprivate(SIZE_OF_COLLECTION)
	for (i = 0; i < 1000; i++)
	{
		SIZE_OF_COLLECTION = i;
		printf("The value of size is %d\n", SIZE_OF_COLLECTION);
	}

	printf("outside the parallel region %d", SIZE_OF_COLLECTION);
	*/
}




