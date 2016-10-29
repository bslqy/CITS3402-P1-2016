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
#include <omp.h>

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

/** individual DIA_SET **/
struct DIA_SET {
	int* collection;
	int col_index;
	int size;
};

long long* KEYS;   //GLOBAL KEY STORAGE

				   //  GLOBAL VARIAABLES USED FOR BLOCK CREATION
struct block b;             //  RECYCLCABLE SINGLE BLOCK
struct block* Big_Block;    //  THE COLLECTION OF ALL BLOCK
int NUM_OF_BLOCK = 0;       //  TOTAL NUMBER OF BLOCK
int MEM_SIZE = 1;			//  MALLOC SIZE OF BLOCK



							//  GLOBAL VARIABLE USED FOR DIA SET
struct DIA_SET one_dia_set;
struct DIA_SET dias[100000];			//  THE COLLECTION OF ALL DIA SET
int NUM_OF_DIA_SET = 0;


/** Function declaration **/

double** readMatrix(char* fileName);
void readKey(char* fileName);
double* readCol(double** jz, int col_number, int row_size);
void collection(double* one_column, int col_num, int start, int row_size);
void add_to_dia_set(int* collection, int col_number, int size);

int get_combination_size(int n, int m);
int** combNonRec(int collect[], int sizeOfColl, int sizeOut);
long long getKey(int row_index);
void create_Block(int arr[], int COL_INDEX);
void add_To_Block_Collection();


/***********************************************************************************/
double** readMatrix(char* fileName) {

	FILE *fp;
	int i, j;
	char s[MAX], ch = "";

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

	double** jz;

	if ((jz = (double **)calloc(row, sizeof(double*))) != NULL) {		//Allocate spaces

		for (i = 0; i < row; i++) {
			jz[i] = (double *)calloc(col, sizeof(double));
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
	}
	return jz;
}
void readKey(char* fileName) {

	if ((KEYS = calloc(4400, sizeof(long long))) != NULL) {


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

	else {
		printf("Memory allocaiton problem 22222222\n");
	}

}

/** FUNCTION THAT OBTAIN THE CORRESPOINDING KEY GIVEN A ROW NUMBER **/
long long getKey(int row_index) {
	return KEYS[row_index];
}

/** use to return one col of the matrix after the col is scanned for the first time */
double* readCol(double** jz, int col_number, int row_size) {

	double* col;

	if ((col = (double*)calloc(row_size, sizeof(double))) != NULL) {
		int i = 0;
		for (i; i < row_size; i++) {
			col[i] = jz[i][col_number];
		}
	}

	return col;
}

/** use to return a collection of DIA neighbours given a start row  */
void collection(double* one_column, int col_num, int start, int row_size) {
	// Put itself in a collection e.g. {1}
	int size_of_set = 1;
	int* collection_for_one_row = malloc(size_of_set * sizeof(int));

	collection_for_one_row[0] = start;

	int i = start;
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

	//SIZE_OF_COLLECTION = size_of_set;  // MODIFY THE ARRAY SIZE N FOR COMBINATION.
	// printf("Inside the collection function, what is the SIZE_OF_COLLECTION? %d\n",SIZE_OF_COLLECTION);
	if (size_of_set >= 4)
	{
		add_to_dia_set(collection_for_one_row, i, size_of_set);
	}

}


void add_to_dia_set(int* collection, int col_number, int size)
{
	//SEQUENTIAL ACCESS
	one_dia_set.collection = collection;
	one_dia_set.col_index = col_number;
	one_dia_set.size = size;
	dias[NUM_OF_DIA_SET] = one_dia_set; // STORE THE INSTANCE OF BLOCK INTO THE COLLECTION, one_dia_set IS THE RECYCLABLE Dia WHICH GETS FREED EVERYTIME
	NUM_OF_DIA_SET++; //ADD ONE MORE Dia TO THE COLLECTION OF Dias
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

	i = 0;
	for (i; i < size_of_combination; i++) {
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
		if ((Big_Block = (struct block*) calloc(MEM_SIZE, sizeof(struct block))) != NULL) {
			Big_Block[NUM_OF_BLOCK] = b;// STORE THE INSTANCE OF BLOCK INTO THE COLLECTION , b IS THE RECYCLABLE BLOCK WHICH GETS FREED EVERYTIME
			NUM_OF_BLOCK++;
		}
		else {
			printf("First calloc unsuccessful\n");
		}
	}

	else if (NUM_OF_BLOCK == MEM_SIZE) {
		//SEQUENTIAL ACCESS
		MEM_SIZE = MEM_SIZE * 2;
		if ((Big_Block = (struct block*)realloc(Big_Block, MEM_SIZE * sizeof(struct block))) != NULL) {
			Big_Block[NUM_OF_BLOCK] = b;
			NUM_OF_BLOCK++; //ADD ONE MORE BLOCK TO THE COLLECTION OF BLOCKS
		}
		else {
			printf("Realloc failed\n");
			exit(1);
		}
	}

	else {
		Big_Block[NUM_OF_BLOCK] = b; // STORE THE INSTANCE OF BLOCK INTO THE COLLECTION, b IS THE RECYCLABLE BLOCK WHICH GETS FREED EVERYTIME
		NUM_OF_BLOCK++; //ADD ONE MORE BLOCK TO THE COLLECTION OF BLOCKS
	}
}


/** FINAL OUTPUT **/
void collision() {
	//  FIXING THE ROW AND COMPARE THE SIGNATURE DOWNWARD, SAME AS FINDING DIA NEIGHBOUR
	int i = 0;
	int j = i + 1;
	int num_of_collision = 0;

	for (i = 0; i < NUM_OF_BLOCK - 1; i++) {
		#pragma omp parallel for shared(NUM_OF_BLOCK,i)
		for (j = i + 1; j <= NUM_OF_BLOCK - 1; j++) {
			if ((Big_Block[i].signature == Big_Block[j].signature) && (Big_Block[i].col_index != Big_Block[j].col_index)) {
				printf("Block %d from Col %d and Block %d from Col %d collide\n", i, Big_Block[i].col_index, j, Big_Block[j].col_index);
				num_of_collision++;
				//	printf("Row Index From Block %d: %d %d %d %d \n", i, Big_Block[i].row1, Big_Block[i].row2, Big_Block[i].row3, Big_Block[i].row4);
				//	printf("Row Index From Block %d: %d %d %d %d \n", j, Big_Block[j].row1, Big_Block[j].row2, Big_Block[j].row3, Big_Block[j].row4);

			}
		}
	}
	printf("Inside the collision funciton, the number of block is %d\n", NUM_OF_BLOCK);
	printf("The Number Of Collision is %d\n", num_of_collision);

}

/******** ======== *********/


int main(void) {



	clock_t begin = clock();

	readKey("keys.txt");
	double** jz = readMatrix("data.txt");
	int col = 0;
	int row = 0;
	int k = 0;



	//#pragma omp parallel for firstprivate(i) shared(Big_Block,NUM_OF_BLOCK,SIZE_OF_COLLECTION,b)

	#pragma omp parallel for
	for (col = 0; col < 499; col++) {

		printf("THIS IS COLUMN %d\n", col);

		double*c = readCol(jz, col, 4400);

		#pragma omp parallel for shared(col)
		for (row = 0; row < 4400; row++) {
			// printf("This is start row %d from column %d !!!!!!!!!!\n", row,col);
			collection(c, col, row, 4400);
		}

		free(c);

	}
	printf("NUM_OF_DIA_SET is %d\n", NUM_OF_DIA_SET);
	/*BLOCK GENERATION*/


	int dia_order = 0;

//	#pragma omp parallel for shared(NUM_OF_DIA_SET)
	for (dia_order = 0; dia_order < NUM_OF_DIA_SET; dia_order++)
	{
		int size = dias[dia_order].size;
		int combination_size = get_combination_size(dias[dia_order].size, M);


		//GET THE 2D-ARRAY OF COMBINATION
		int** rrr = combNonRec(dias[dia_order].collection, dias[dia_order].size, M);
	//	#pragma omp parallel for shared(NUM_OF_DIA_SET,dia_order)
		for (k = 0; k < combination_size; k++) {
			create_Block(rrr[k], dias[dia_order].col_index);   //ACTUAL CREATION OF BLOCK !!!!!!!
															   //printf("This is block %d \n", NUM_OF_BLOCK);
			add_To_Block_Collection();

		}
		free(rrr);
	}


	collision();


	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Total time spent : %f\n", time_spent);

	printf("The size of block is %d", NUM_OF_BLOCK);

	free(jz);
	free(Big_Block);
	free(KEYS);
	exit(0);

}





