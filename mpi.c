
/*
* Student Number1: 21448107
* Student Name1: Cikang Li
*
* Student Number2: 21480279
* Student Name2: Jia Liao
*
*/

/* BREAKING INTO STEPS*/

/* STEP 1	Reading and storing matrix

FUNCTION USED:
double** readMatrix(char* fileName);
void readKey(char* fileName);
*/

/* STEP 2	Pick one column (First level of parellelization)

Function Used:
double* readCol(double** jz, int col_number, int row_size);   OpenMP is used.
*/

/* STEP 3	Fixing one row	(Second level of parellelization)
and try to find the int[] which contains the row index of a neighbourhood, the size and the col index
All the information are recording in struct DIA_SET one_dia_set (a placeholder) and then stored in struct DIA_SET dias[100000]
Information in the one_dia_set is overwritten when then next neighbourhood is found.

Function Used:
void collection(double* one_column, int col_num, int start, int row_size);  OpenMP is used.
void add_to_dia_set(int* collection, int col_number, int size);
*/


/* STEP 4	Iterate through the dias[100000] , used dias[index].collection(an int[] with size = dias[index].size )  to figure out
all the combination for that int[].
The result will be an int[][4]. For example, {1,2,3,4,5} -> {{1,2,3,4},{1,2,3,5},{1,2,4,5},{2,1,3,4}....}
The int get_combination_size(int n, int m) function returns the number of combination.
Summing up all the combination size give the final NUM_OF_BLOCK;
Allocate the memory for struct block Big_Block once only. Reduce the likelyhood of realloc failure.

Function Used:
int get_combination_size(int n, int m);
int** combNonRec(int collect[], int sizeOfColl, int sizeOut);	   OpenMP is used.

*/
/* STEP 5	Creation of individual blocks by calculating the signature and then store it into struct Big_Blocks*
In

Function Used:
long long getKey(int row_index);
void create_Block(int arr[], int COL_INDEX);    OpenMP is used.
void add_To_Block_Collection();

*/


/* STEP 6
Do the collision. Basically a Brute-force iteration. Fixing one block and matching downward. Total number of times will be N(N+1)/2
Function Used:
Collsion();   OpenMP is used.

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include "mpi.h"

#define MAX 1000000  //one row of double = sizeof(Double) *4400
#define ROW_SIZE 4400
#define COL_SIZE 500
#define DIA 0.0000011
#define M  4        // SIZE OF COMBINATION
#define MASTER 0	  /* taskid of first task */
#define FROM_MASTER 1 /* setting a message type */
#define FROM_WORKER 2 /* setting a message type */

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
	int collection[80];
	int col_index;
	int size;
};



long long* KEYS;   //GLOBAL KEY STORAGE

				   //  GLOBAL VARIAABLES USED FOR BLOCK CREATION
struct block b;             //  RECYCLCABLE SINGLE BLOCK
struct block* Big_Block;    //  THE COLLECTION OF ALL BLOCK
int NUM_OF_BLOCK = 0;       //  TOTAL NUMBER OF BLOCK
int BLOCK_INDEX = 0;			//  MALLOC SIZE OF BLOCK



struct DIA_SET one_dia_set;			//  GLOBAL VARIABLE USED FOR DIA SET (neighbours)
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

/** use to return a collection of DIA neighbours given a fixed row
START is the benchmark of the dia checking operation
i.e. fixing START and find the DIA down the row

*/
void collection(double* one_column, int col_num, int start, int row_size) {
	// Put itself in a collection e.g. {1}
	int size_of_set = 1;
	int* collection_for_one_row = malloc(size_of_set * sizeof(int));
	collection_for_one_row[0] = start;
	int i = start;

#pragma omp parallel for shared(collection_for_one_row)
	for (i = start; i < row_size - 2; i++) {
		//Put itself in a collection e.g. {START, START+1,START+2 ...}
		if (fabs(one_column[start] - one_column[i + 1]) < DIA) {    //If two row are in the same DIA  //Put the row index in an array
#pragma atomic
			size_of_set++;
			collection_for_one_row = (int*)realloc(collection_for_one_row, size_of_set * sizeof(int)); //Resize the array, increase by one.
			collection_for_one_row[size_of_set - 1] = i + 1; // Add the index of the neighbour into the dia array.
		}
	}

	//SIZE_OF_COLLECTION = size_of_set;  // MODIFY THE ARRAY SIZE N FOR COMBINATION.
	// printf("Inside the collection function, what is the SIZE_OF_COLLECTION? %d\n",SIZE_OF_COLLECTION);

	//Only add to the big dias[10000] if the size if more than 4;
	if (size_of_set >= 4)
	{
		add_to_dia_set(collection_for_one_row, col_num, size_of_set);
	}

}


void add_to_dia_set(int* set, int col_number, int size)
{
	// Passing the pointer values to the array, because pointer cannot be passed in MPI
	one_dia_set.col_index = col_number;
	for (int i = 0; i<size; i++) {
		one_dia_set.collection[i] = set[i];
	}
	one_dia_set.size = size;
	dias[NUM_OF_DIA_SET] = one_dia_set; // Store the instance of DIA_SET into dias[100000]
	NUM_OF_DIA_SET++; //Increment the count	
}


/** modify the size of nCm for different input set **/
/** nCm = n!/m!(n-m)! **/
/** Use natural log to reduce the size **/

int get_combination_size(int n, int m) {
	return  floor(round(exp(lgamma(n + 1) - lgamma(n - m + 1)) / tgamma(m + 1)));
}

// Combination process. Input is an int[] with size >4 and the output is int[][4] with all the combinations
int** combNonRec(int collect[], int sizeOfColl, int sizeOut) {


	int size_of_combination = get_combination_size(sizeOfColl, sizeOut);
	int index, i, *p;
	int** finalR;
	if ((finalR = (int**)calloc(size_of_combination, sizeof(int*))) != NULL) {

		i = 0;
		for (i; i < size_of_combination; i++) {
			finalR[i] = (int*)calloc(4, sizeof(int));
		}

		if ((p = (int*)calloc(sizeOut, sizeof(int))) != NULL) {

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
		}

	}
	else {
		printf("Memory allocaiton problem 44444444\n");
	}


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

	Big_Block[BLOCK_INDEX] = b; // STORE THE INSTANCE OF BLOCK INTO THE COLLECTION, b IS THE RECYCLABLE BLOCK WHICH GETS FREED EVERYTIME

}


/** FINAL OUTPUT **/
void collision() {
	//  FIXING THE ROW AND COMPARE THE SIGNATURE DOWNWARD, SAME AS FINDING DIA NEIGHBOUR
	int i = 0;
	int j = i + 1;
	int num_of_collision = 0;

#pragma omp parallel for shared(NUM_OF_BLOCK,num_of_collision)
	for (i = 0; i < NUM_OF_BLOCK - 1; i++) {
#pragma omp parallel for shared(NUM_OF_BLOCK,i,num_of_collision)
		for (j = i + 1; j <= NUM_OF_BLOCK - 1; j++) {
			if ((Big_Block[i].signature == Big_Block[j].signature) && (Big_Block[i].col_index != Big_Block[j].col_index)) {
				//printf("Block %d from Col %d and Block %d from Col %d collide\n", i, Big_Block[i].col_index, j, Big_Block[j].col_index);
#pragma omp atomic	
				num_of_collision++;
				//	printf("Row Index From Block %d: %d %d %d %d \n", i, Big_Block[i].row1, Big_Block[i].row2, Big_Block[i].row3, Big_Block[i].row4);
				//	printf("Row Index From Block %d: %d %d %d %d \n", j, Big_Block[j].row1, Big_Block[j].row2, Big_Block[j].row3, Big_Block[j].row4);

			}
			else {
				// printf("No collision Happens for Block %d and Block %d \n",i,j);
			}
		}
	}
	printf("Inside the collision funciton, the number of block is %d\n", NUM_OF_BLOCK);
	printf("The total number of collision is %d\n", num_of_collision);
}

/******** ======== *********/


int main(int argc, char* argv[]) {

	clock_t begin = clock();

	readKey("keys.txt");
	double** jz = readMatrix("data.txt");

	int numtasks, /* number of tasks in partition */
		taskid, /* a task identifier */
		numworkers, /* number of worker tasks */
		source, /* task id of message source */
		dest, /* task id of message destination */
		mtype, /* message type */
		cols, /* rows of matrix A sent to each worker */
		avecol, extra, offset, /* used to determine rows sent
							   to each worker */
		i, j, k, rc; /* misc */

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	if (numtasks < 2) {
		printf("Need at least two MPI tasks. Quitting...\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
		exit(1);
	}
	numworkers = numtasks - 1;

	/* Create a DIA_SET structure */
	const int nitems = 3;
	int          blocklengths[3] = { 80,1,1 };
	MPI_Datatype types[3] = { MPI_INT, MPI_INT, MPI_INT };
	MPI_Datatype mpi_dia_set_type;
	MPI_Aint     offsets[3];

	offsets[0] = offsetof(struct DIA_SET, collection);
	offsets[1] = offsetof(struct DIA_SET, col_index);
	offsets[2] = offsetof(struct DIA_SET, size);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_dia_set_type);
	MPI_Type_commit(&mpi_dia_set_type);


	if (taskid == MASTER)
	{
		printf("mpi_mm has started with %d tasks.\n", numtasks);
		printf("Initializing arrays...\n");

		/* Send matrix data to the worker tasks */
		avecol = COL_SIZE / numworkers;
		extra = COL_SIZE % numworkers;
		offset = 0;
		mtype = FROM_MASTER;
		int dia_index = 0;

			for (dest = 1; dest <= numworkers; dest++)
			{

				cols = (dest <= extra) ? avecol + 1 : avecol;
				printf("Sending %d cols to task %d offset=%d\n",
					cols, dest, offset);

				// CURRENT OFFSET 
				MPI_Send(&offset, 1, MPI_INT, dest,
					mtype, MPI_COMM_WORLD);

				// HOW MANY COLS TO BE SENT TO EACH PROCESS?
				MPI_Send(&cols, 1, MPI_INT, dest,
					mtype, MPI_COMM_WORLD);

				// ACTUAL DATA TO BE SENT
				MPI_Send(&jz[0][offset], cols*ROW_SIZE, MPI_DOUBLE,
					dest, mtype, MPI_COMM_WORLD);

				offset = offset + cols;
			}


		/* Receive results from worker tasks */
		mtype = FROM_WORKER;
		for (i = 1; i <= numworkers; i++)
		{
			source = i;

			// CURRENT OFFSET 
			MPI_Recv(&offset, 1, MPI_INT, source, mtype,
				MPI_COMM_WORLD, &status);

			// HOW MANY COLS ALREADY PROCESSED?
			MPI_Recv(&cols, 1, MPI_INT, source, mtype,
				MPI_COMM_WORLD, &status);

			// WHAT IS THE NUM OF DIA IN EACH INDIVIDUAL PROCESS
			MPI_Recv(&NUM_OF_DIA_SET, 1, MPI_INT, MASTER, mtype,
				MPI_COMM_WORLD, &status);


			// Receiving DIA_SET elements starting from 0, using an index to correclty fill the dias[100000]
			MPI_Recv(&dias[dia_index], NUM_OF_DIA_SET, mpi_dia_set_type,
				dest, mtype, MPI_COMM_WORLD, &status);

			dia_index += NUM_OF_DIA_SET; //Increase to dia_index to indicate the correct place in dias[10000];


			printf("Received results from task %d\n", source);
		}



		/**************************** worker task ************************************/
		if (taskid > MASTER)
		{
			mtype = FROM_MASTER;

			// START OF THE COLUMN
			MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype,
				MPI_COMM_WORLD, &status);

			//HOW MANY COLS TO PROCESS?
			MPI_Recv(&cols, 1, MPI_INT, MASTER,
				mtype, MPI_COMM_WORLD, &status);

			// COL PARTITION
			MPI_Recv(&jz, cols*ROW_SIZE, MPI_DOUBLE, MASTER,
				mtype, MPI_COMM_WORLD, &status);

			//CURRENT dia_index
			MPI_Recv(&dia_index, 1, MPI_INT,
				dest, mtype, MPI_COMM_WORLD, &status);



			//Process the data
			//#pragma omp parallel for
			for (int i = offset; i < offset + cols; i++) {
				printf("THIS IS COLUMN %d\n", i);
				double*c = readCol(jz, i, 4400);

				//#pragma omp parallel for shared(i)
				for (j = 0; j < 4400; j++) {
					// printf("This is fixed row %d from column %d !!!!!!!!!!\n",j,i);
					collection(c, i, j, 4400);
				}
				free(c);
			}

			/* Three "variables in struct DIA_SET " are used
			struct DIA_SET one_dia_set;
			struct DIA_SET dias[100000];
			int NUM_OF_DIA_SET = 0;
			*/

			mtype = FROM_WORKER;
			MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);

			MPI_Send(&cols, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);

			// Send the DIA_SET elements size to master so the master's dias[100000] could use an index to fill the incoming DIA_SET elements correctly
			//The NUM_OF_DIA_SET is different from each process
			MPI_Send(&NUM_OF_DIA_SET, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);

			//	Send the DIA_SET elements to the master's dia[100000] , with NUM_OF_DIA_SET equals to the individual process's calculation
			MPI_Send(&dias, NUM_OF_DIA_SET, mpi_dia_set_type, MASTER, mtype, MPI_COMM_WORLD);


		}
		MPI_Finalize();

	}


	/*BLOCK GENERATION*/

	int dia_index = 0; // NOTE THAT dias[10000] is an array that holds all the relationships.
#pragma omp parallel for shared(NUM_OF_DIA_SET,NUM_OF_BLOCK) 

					   // FIRST ROUND : CALCULATE THE TOTAL NUMBER OF BLOCK AND THE ALLOCATE THE SPACE ACCORDINGLY
	for (dia_index = 0; dia_index < NUM_OF_DIA_SET; dia_index++)
	{
		int size = dias[dia_index].size;
		int combination_size = get_combination_size(dias[dia_index].size, M);
		//GET THE 2D-ARRAY OF COMBINATION
#pragma omp parallel for shared(NUM_OF_DIA_SET,NUM_OF_BLOCK,dia_index,combination_size)
		for (k = 0; k < combination_size; k++) {
#pragma omp atomic
			NUM_OF_BLOCK++;
		}
	}

	printf("NUM_OF_BLOCK is %d\n", NUM_OF_BLOCK);


	// SECOND ROUND : ALLOCATE THE TOTAL NUMBER OF BLOCK AT ONCE
	if ((Big_Block = (struct block*)calloc(NUM_OF_BLOCK, sizeof(struct block))) != NULL)
	{
		for (dia_index = 0; dia_index < NUM_OF_DIA_SET; dia_index++)
		{
			int** rrr = combNonRec(dias[dia_index].collection, dias[dia_index].size, M);

#pragma omp parallel for shared(NUM_OF_DIA_SET,NUM_OF_BLOCK,dia_index,BLOCK_INDEX)

			for (k = 0; k < get_combination_size(dias[dia_index].size, M); k++) {
#pragma omp critical
				{
					create_Block(rrr[k], dias[dia_index].col_index);   //ACTUAL CREATION OF BLOCK !!!!!!!
					add_To_Block_Collection();							//ADD THE BLOCK INTO THE BIG_BLOCK
																		//printf("This is block %d\n", BLOCK_INDEX);
					BLOCK_INDEX++;										//Moinvg to the next empty space of BIG_BLOCK
				}
			}

			free(rrr);
		}
	}

	else {
		printf("Calloc for big_Block fails\n");
		exit(1);
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




