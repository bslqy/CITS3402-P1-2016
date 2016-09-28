#include <stdlib.h>
#include <stdio.h>
#define MAX 1000000  //one row of double = sizeof(Double) *4400
#define LINE_LENGTH 15

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
      fgetc(fp);

      printf("\nline[%d] = %s\n", i+1, string);
      strcpy(keys[i],string);

      i++;
    }


printf("%s hga",&keys[0]);
  fclose(fp);

}



void readMatrix(char* fileName)
{
	FILE *fp;
	int i, j;
	char s[MAX], ch;




	if ((fp = fopen(fileName, "r")) == NULL) {
    printf("can not open this file!\n");
    exit(1);
	}
  int row = 0;
  while (fgets(s, MAX, fp) != NULL) //Count the line
    row++;

  rewind(fp); //Going back to the head of the matrix

  int col = 0; //Count the col
  while (ch != '\n') //(ch=fgetc(fp))!='\n'&&(ch=fgetc(fp))!='\r'
  {
    if (ch == ' ') col++;
    ch = fgetc(fp);
  }
  col++; //

  double** jz = malloc(row * sizeof(double*)); //Allocate spaces
  for (i = 0; i < row; i++)
    jz[i] = malloc(col * sizeof(double));

  rewind(fp);

  for (i = 0; i < row; i++) //Read in the data
    for (j = 0; j < col; j++) {
      if (!fscanf(fp, "%lf", &jz[i][j])) {
        break;
      }
    }

  for (i = 0; i < row; i++) //Print the matrix
    for (j = 0; j < col; j++) {
      printf("%lf\t", jz[i][j]);
      if (j + 1 == col) printf("\n");
    }
  fclose(fp);

  printf("%d row, %d  col ",row,col);
    }



int main(void) {


readKey("keys.txt");

}
