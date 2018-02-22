#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define numThreads 11
/* You will need a structure to store the information to be passed to each thread (see "Passing Parameters to Each Thread" in the textbool's project description)*/
// Hell world

typedef struct
{
	int r;
	int c;
} parameters;
/* You will need to declare an array of integer values that is visible to each thread. The value in the array (0 or 1) indicates whether the worker thread's number is valid (see "Returning Results to the Parent Thread" in the textbook's project description) */
int valid[numThreads]={0};
int sudoku[9][9];
/* Declare the thread that checks columns */
void *column_worker(void *param){
	parameters *data=(parameters *) param;
	int row=data->r;
	int col=data->c;
	int i,j,k;
	int isValidCol[9]={0};
	if (row!=0 || col!=0){
		printf("Invalid row and column to check\n");
		pthread_exit(NULL);
	}
	for (i=0;i<9;i++){
		for(k=0;k<9;k++) isValidCol[k]=0;
		for (j=0;j<9;j++){
			int numb=sudoku[j][i];
			if(numb<1 || numb>9 || isValidCol[numb-1]==1)
				{pthread_exit(NULL);}
			else
				{isValidCol[numb-1]=1;}
		}
	}
	//printf("1\n");
	valid[9]=1;
	pthread_exit(NULL);
} 	// the function parameter is for the structure you defined
/* You will also need to declare threads that checks rows and 3x3 subgrids */
void *row_worker(void *param){
	parameters *data=(parameters *) param;
	int row=data->r;
	int col=data->c;
	if (row!=0 || col!=0){
		printf("Invalid row and column to check\n");
		pthread_exit(NULL);
	}
	int isValidRow[9]={0};
	int i,j,k;
	for (i=0;i<9;i++){
		for(k=0;k<9;k++) isValidRow[k]=0;
		for (j=0;j<9;j++){
			int numb=sudoku[i][j];
			if(numb<1 || numb>9 || isValidRow[numb-1]==1)
				{pthread_exit(NULL);}
			else
				{isValidRow[numb-1]=1;}
		}
	}
	//printf("1\n");
	valid[10]=1;
	pthread_exit(NULL);
}
void *subgrid_worker(void *param){
	parameters *data=(parameters *) param;
	int row=data->r;
	int col=data->c;
	int isValidSubGrid[9]={0};
	int i,j,k;
	//printf("%d %d\n",row,col);
	if (row%3!=0 || col%3!=0 || row<0 || row>6|| col<0|| col>6){
		printf("Invalid row and column to check\n");
		pthread_exit(NULL);
	}
	for (i=row;i<row+3;i++){
		for(j=col;j<col+3;j++){
			int numb=sudoku[i][j];
			if(numb<1 || numb>9 || isValidSubGrid[numb-1]==1)
				{pthread_exit(NULL);}
			else
				{isValidSubGrid[numb-1]=1;}
		}
	}
	//printf("1\n");
	valid[row+col/3]=1;
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	/*You need to assign values to the structure variable. Then you can create multiple worker threads by passing the information using the structure variable*/
	if (argc!=2){
		return -1;
	}
	FILE *fp;
	fp=fopen(argv[1],"r");
	char ch;
	if (fp==NULL){
		printf("Error! Could not open file\n");
		exit(-1);
	}
	int i=0,j=0;
	for (i=0;i<9;i++){
		fscanf(fp,"%d %d %d %d %d %d %d %d %d\n",&sudoku[i][0],&sudoku[i][1],&sudoku[i][2],&sudoku[i][3],&sudoku[i][4],&sudoku[i][5],&sudoku[i][6],&sudoku[i][7],&sudoku[i][8]);
	}
	fclose(fp);
	printf("Printing the sudoku:\n");
	for (i=0;i<9;i++){
		for(j=0;j<9;j++){
			printf("%d ",sudoku[i][j]);
		}
		printf("\n");
	}
	pthread_t tid[numThreads];
	pthread_attr_t attr;
	int threadidx=0;
	//threads for sub 3x3 grids workers
	for (i=0;i<9;i++){
		for(j=0;j<9;j++){
			if (i%3==0 && j%3==0){
				parameters *subGridData=(parameters*)malloc(sizeof(parameters));
				subGridData->r=i;
				subGridData->c=j;
				pthread_create(&tid[threadidx++],NULL,subgrid_worker,subGridData);
			}
		}
	}
	//threads for column and row workers
	parameters *columnData=(parameters*)malloc(sizeof(parameters));
	columnData->r=0;
	columnData->c=0;
	pthread_create(&tid[threadidx++],NULL,column_worker,columnData);
	parameters *rowData=(parameters*)malloc(sizeof(parameters));
	rowData->r=0;
	rowData->c=0;
	pthread_create(&tid[threadidx++],NULL,row_worker,rowData);
	/*You need to call pthread_join() for each childer thread so that the parent will wait*/
	for (i=0;i<numThreads;i++) pthread_join(tid[i],NULL);
	/* Finally, after all children returns, you can check the status array that is visible to everyone and see if it is valid. You then print out the final checking result*/
	//for (i=0;i<numThreads;i++)
	//		printf("%d ",valid[i]);
	printf("RESULT: ");
	for (i=0;i<numThreads;i++){
		if(valid[i]==0){
			printf("The solution for this sudoku is INVALID!\n");
			return 0;
		}
	}
	printf("The solution for this sudoku is VALID!\n");
	return 0;
}
