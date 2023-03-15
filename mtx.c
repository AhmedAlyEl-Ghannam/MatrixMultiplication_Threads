/**
 * Included Libraries/Headers
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
/*********************END OF HEADERS/MACROS*****************************/

/**
 *Global Variables
 */
 
/*
 * Main Matrix Files
 */
FILE * File_matrixA;
FILE * File_matrixB;
FILE * File_matrixC;

/*
 * Matrices + Dimensions
 */
int** matrixA ;
int matrixA_rows = 0;
int matrixA_cols = 0;

int **matrixB;
int matrixB_rows = 0;
int matrixB_cols = 0;

int** matrixC;

/*
 * Matrix File Characteristics 
 */
typedef struct {
    char* file_name;
    char signature;
} MatrixFileCCTs;

unsigned long int buffer = 900000;
/*********************END OF GLOBAL VARIABLES*****************************/

/**
 * Function Prototypes
 */
 // Read Matrix from a File
void* readMatrixFile(MatrixFileCCTs* file_pointer);
// Assign Approriate Space for Output Matrix C
void allocateOutputMatrixC(void);
// Multiply a & b using a Single Thread (Method 01)
void threadPerMTXMul(void);
// Multiply a & b using a Thread Per Row (Method 02)
void threadPerRowMTXMul(void);
// Method 02's Thread Routine
void* threadPerRowMTXMul_ROUTINE(void* file_pointer);
// Multiply a & b using a Thread Per Element(Method 03)
void threadPerElementMTXMul(void);
// Method 03's Thread Routine
void* threadPerElementMTXMul_ROUTINE(void* file_pointer);
/*********************END OF FUNCTION PROTOTYPES*****************************/

/**
 * Main Function
 */

int main(void) 
{	 
		long int p, q;
		// Variables to Calculate Runtime for each Method
		struct timeval start, stop;
		
		// Defining Each Matrix File and its Characteristics 
		MatrixFileCCTs matrixA_FileCCTs;
		matrixA_FileCCTs.file_name = "a.txt";
		matrixA_FileCCTs.signature = 'a' ;
		
		MatrixFileCCTs matrixB_FileCCTs;
		matrixB_FileCCTs.file_name = "b.txt";
		matrixB_FileCCTs.signature = 'b' ;
		
		MatrixFileCCTs matrixC_FileCCTs;
		matrixC_FileCCTs.file_name = "c.txt";
		matrixC_FileCCTs.signature = 'c' ;

		// Reading the Contents of Files a.txt & b.txt Respectively
		readMatrixFile(&matrixA_FileCCTs);
		readMatrixFile(&matrixB_FileCCTs);
		
		// Checking for the Condition of Proper Matrix Multiplication (Must be Equal)
		if(matrixA_cols != matrixB_rows)
		{
				// Open the Already Prepared Output File C and Prepare to Write in it
				File_matrixC = fopen(matrixC_FileCCTs.file_name, "w");
				if(File_matrixC == NULL)
				{
			        printf("C.txt: Output File does not Exist! WTH?! (1)\n");
			        return 0;
			    }
				
			    fprintf(File_matrixC, "Invalid Matrix Multiplication: Unable to Multiply these Matrices!\n"); // fprintf is used when writing in files
			    fclose(File_matrixC);
			    return 0;
		}
		
		// Assign Space for Matrix C
		allocateOutputMatrixC();

		/*
		 * Method 01
		 * Store both the Start and End Times for Method 01
		 */
		gettimeofday(&start,NULL); // Start
		threadPerMTXMul(); // Method 01
		gettimeofday(&stop,NULL); // End

	    File_matrixC = fopen(matrixC_FileCCTs.file_name,"w");
	    if(File_matrixC == NULL)
		{
				printf("C.txt: Output File does not Exist! WTH?! (2)\n");
				return 0;
	    }
	    fprintf(File_matrixC,"MATRIX C (Output)\n");
	    fprintf(File_matrixC,"\n");
	    fclose(File_matrixC);
		
		// Execution Time in usec
	    long int execTime_us_method1 = stop.tv_usec - start.tv_usec;
		// Store the Output Matrix in File c.txt + Execution Time + No. of Threads
	    File_matrixC = fopen(matrixC_FileCCTs.file_name,"a+");
	    fprintf(File_matrixC,"Method 01 (Single Thread)\n");
	    fprintf(File_matrixC,"\n");
		fprintf(File_matrixC,"-------------------------------\n");
	    for(p = 0; p < matrixA_rows; p++)
		{
				for(q = 0; q < matrixB_cols; q++)
						fprintf(File_matrixC,"%d ",matrixC[p][q]);
					
	           fprintf(File_matrixC,"\n");
	    }
		
	    fprintf(File_matrixC,"\n");
		fprintf(File_matrixC,"-------------------------------\n");
	    fprintf(File_matrixC,"Execution Time = %lu usec\n",execTime_us_method1);
	    fprintf(File_matrixC,"\n");
		fprintf(File_matrixC,"-------------------------------\n");
	    fprintf(File_matrixC,"Number of Threads = Just One :D");

	    fprintf(File_matrixC,"\n");
		fprintf(File_matrixC,"=====================================\n");
		/*********************END OF METHOD 01*****************************/
		
		/*
		 * Method 02
		 * Store both the Start and End Times for Method 02
		 */
	    gettimeofday(&start,NULL);
	    threadPerRowMTXMul();
	    gettimeofday(&stop,NULL);
		
		// Execution Time in usec
	    long int execTime_us_method2 = stop.tv_usec - start.tv_usec;
		// Store the Output Matrix in File c.txt + Execution Time + No. of Threads
	    File_matrixC = fopen(matrixC_FileCCTs.file_name, "a+");
	    fprintf(File_matrixC,"Method 02 (Thread Per Row) \n");
	    fprintf(File_matrixC,"\n");
		fprintf(File_matrixC,"-------------------------------\n");
	    for(p = 0; p < matrixA_rows; p++)
		{
				for(q = 0; q < matrixB_cols; q++)
						fprintf(File_matrixC,"%d ",matrixC[p][q]);
	       	           
	       	    fprintf(File_matrixC,"\n");
				
	    }
	    fprintf(File_matrixC,"\n");
		fprintf(File_matrixC,"-------------------------------\n");
	    fprintf(File_matrixC,"Execution Time = %lu usec\n",execTime_us_method2);
	    fprintf(File_matrixC,"\n");
		fprintf(File_matrixC,"-------------------------------\n");
	    fprintf(File_matrixC,"Number of Threads = %d\n",matrixA_rows);
		
	    fprintf(File_matrixC,"\n");
	    fprintf(File_matrixC,"=====================================\n");
		/*********************END OF METHOD 02*****************************/

		/*
		 * Method 03
		 * Store both the Start and End Times for Method 03
		 */
	    gettimeofday(&start,NULL);
	    threadPerElementMTXMul();
	    gettimeofday(&stop,NULL);
		
		// Execution Time in usec
	    long int execTime_us_method3 = stop.tv_usec - start.tv_usec;
		// Store the Output Matrix in File c.txt + Execution Time + No. of Threads
	    File_matrixC = fopen(matrixC_FileCCTs.file_name,"a+");
	    fprintf(File_matrixC,"Method 03 (Thread Per Element) \n");
	    fprintf(File_matrixC,"\n");
		fprintf(File_matrixC,"-------------------------------\n");
	    for(p = 0; p < matrixA_rows; p++)
		{
	    	    for(q = 0; q < matrixB_cols; q++)
	    	      	    fprintf(File_matrixC, "%d ",matrixC[p][q]);
	    	      	       	           
	    	    fprintf(File_matrixC, "\n");
	    }
	    fprintf(File_matrixC, "\n");
		fprintf(File_matrixC, "-------------------------------\n");
	    fprintf(File_matrixC, "Execution Time = %lu usec\n",execTime_us_method3);
	    fprintf(File_matrixC, "\n");
		fprintf(File_matrixC,"-------------------------------\n");
	    fprintf(File_matrixC, "Number of Threads = %d\n",matrixA_rows*matrixB_cols);
	    fprintf(File_matrixC, "\n");
	    fprintf(File_matrixC,"=====================================\n");
		/*********************END OF METHOD 03*****************************/
		
		// Free Space Occupied by a, b, & c Before Exiting
	    for (p = 0; p < matrixA_rows; p++)
	    {
	    	    free(matrixA[p]);
	    	    free(matrixC[p]);
	    }
	    for (p = 0; p < matrixB_rows; p++)
	    	    free(matrixB[p]);
			
		return 0;
}
/*********************END OF MAIN*****************************/

/**
 * @FUNC_NAME: readMatrixFile
 * @PARAM: MatrixFileCCTs* (pointer to the struct defined above)
 * @RET_TYPE: void*
 */

void* readMatrixFile(MatrixFileCCTs* file_pointer)
{
	MatrixFileCCTs *dumDumFile = (MatrixFileCCTs *)file_pointer;
    int h, g, d;
    if(dumDumFile->signature == 'a')
	{
			File_matrixA = fopen(dumDumFile->file_name, "r");
			if(File_matrixA == NULL)
			{
					printf("a.txt: Input File does not Exist! WTH?! \n");
					return NULL;
			}
			
			fscanf(File_matrixA,"row=%d col=%d", &matrixA_rows, &matrixA_cols);
			matrixA = malloc(matrixA_rows * sizeof(*matrixA));

        	if(!matrixA) 
        	{
					exit(-1);
        	}
        	for(int i = 0; i < matrixA_rows; i++)
        	{
        		    matrixA[i] = malloc(matrixA_cols * sizeof(**matrixA));
        		    if(!matrixA[i]) /* If `malloc` matrixA_FileCCTsiled */
        		    {
							for(int j = 0; j < i; j++) /* free previously allocated memory */
							{
								free(matrixA[j]);
							}
							free(matrixA);

							exit(-1);
        		    }
        	}
        	char* line = (char*) malloc(buffer);
            int n = 0;
			getline(&line,&buffer,File_matrixA);
        	while(getline(&line,&buffer,File_matrixA) != -1)
			{
        			for (int j=0;j<matrixA_cols;j++)
        					matrixA[n][j] = strtol(line,&line,0) ;
        			
					n++;

        	}
			fclose(File_matrixA);
    } else if(dumDumFile->signature == 'b') {
		
			File_matrixB = fopen(dumDumFile->file_name,"r");
			if(File_matrixB == NULL)
			{
					printf("b.txt: Input File does not Exist! WTH?! \n");
					return NULL;
			}
			fscanf(File_matrixB, "row=%d col=%d", &matrixB_rows, &matrixB_cols);
            matrixB = malloc(matrixB_rows * sizeof(*matrixB));
            
			if(!matrixB) 
            {
                	exit(-1);
            }
			
            for(int i = 0; i < matrixB_rows; i++)
            {
                	matrixB[i] = malloc(matrixB_cols * sizeof(**matrixB));
                	if(!matrixB[i]) 
                	{
                		    for(int j = 0; j < i; j++) 
                		    {
                		            free(matrixB[j]);
                		    }
							
                		    free(matrixB);

                		    exit(-1);
                	}
            }
			
            for(h=0; h < matrixB_rows; h++)
			{
                    for(g=0; g < matrixB_cols; g++)
					{
							matrixB[h][g] = 0 ;
							if(fscanf(File_matrixB, "%d ", &matrixB[h][g]) < 1)
							{
									printf("Error occured while parsing values! in A at i=%d j=%d\n", h, g);
									fclose(File_matrixB);
									return NULL;
							}
                    }
            }
            fclose(File_matrixB);
    } else {
		
        printf("Error Because No!");
        return NULL;
    }
	
    return NULL;
}
/*********************END OF readMatrixFile*****************************/

/**
 * @FUNC_NAME: allocateOutputMatrixC
 * @PARAM: none
 * @RET_TYPE: void
 */
 
void allocateOutputMatrixC(void)
{
		matrixC = malloc(matrixA_rows * sizeof(*matrixC));
	    if(!matrixC) 
	    {
	        	 exit(-1);
	    }
		
	    for(int i = 0; i < matrixA_rows; i++)
	    {
	        	matrixC[i] = malloc(matrixB_cols * sizeof(**matrixC));
	        	if(!matrixC[i]) 
	        	{
	        		    for(int j = 0; j < i; j++) 
	        		            free(matrixC[j]);
							
	        		    free(matrixC);

	        		    exit(-1);
	        	}
	    }
}
/*********************END OF allocateOutputMatrixC*****************************/

/**
 * @FUNC_NAME: threadPerMTXMul
 * @PARAM: none
 * @RET_TYPE: void
 */
void threadPerMTXMul(void)
{ 
		for (int i = 0; i < matrixA_rows; ++i) 
		{
				for (int j = 0; j < matrixB_cols; ++j) 
				{
						matrixC[i][j] = 0;
						
						for (int k = 0; k < matrixA_cols; ++k) 
								matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
				}
	   }
}
/*********************END OF threadPerMTXMul*****************************/

/**
 * @FUNC_NAME: threadPerRowMTXMul_ROUTINE
 * @PARAM: void*
 * @RET_TYPE: void*
 */

void* threadPerRowMTXMul_ROUTINE(void* file_pointer)
{
    long int core = (long int) file_pointer;
	int i, j, k;
    for (i = core; i < core + 1; i++)
	{
			for (j = 0; j < matrixB_cols; j++)
			{
					matrixC[i][j] = 0 ;
					for (k = 0; k < matrixA_cols; k++)
							matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
			}
	}
    pthread_exit(NULL) ;
}
/*********************END OF threadPerRowMTXMul_ROUTINE*****************************/

/**
 * @FUNC_NAME: threadPerRowMTXMul
 * @PARAM: none
 * @RET_TYPE: none
 */

void threadPerRowMTXMul(void)
{ 
		long int i;
		// Declaring Threads
		pthread_t threads[matrixA_rows];

		// Creating Threads
		for (i = 0; i < matrixA_rows; i++)
				pthread_create(&threads[i], NULL, threadPerRowMTXMul_ROUTINE, (void*)(i));
    
		// Joining Threads in a Separate Loop to Prevent Errors
		for (i = 0; i < matrixA_rows; i++)
				pthread_join(threads[i], NULL);
}
/*********************END OF threadPerRowMTXMul*****************************/

/**
 * @FUNC_NAME: threadPerRowMTXMul
 * @PARAM: void*
 * @RET_TYPE: void*
 */
void* threadPerElementMTXMul_ROUTINE(void* file_pointer)
{
    long int core = (long int) file_pointer;
    long int r = core % matrixA_rows;
    long int c = core / matrixA_rows;
	int k;
    matrixC[r][c] = 0;
	
    for (k = 0; k < matrixA_cols; k++)
            matrixC[r][c] += matrixA[r][k] * matrixB[k][c];
    
	pthread_exit(NULL);
}
/*********************END OF threadPerRowMTXMul*****************************/

/**
 * @FUNC_NAME: threadPerElementMTXMul
 * @PARAM: none
 * @RET_TYPE: none
 */
void threadPerElementMTXMul(void)
{
    long int i;
	// Declaring Threads
    pthread_t threads[matrixA_rows * matrixB_cols];

    // Creating Threads
    for (i = 0; i < (matrixA_rows * matrixB_cols); i++)
			pthread_create(&threads[i], NULL, threadPerElementMTXMul_ROUTINE, (void*)(i));

    // Joining Threads in a Separate Loop to Prevent Errors
    for (i = 0; i < matrixA_rows*matrixB_cols; i++)
			pthread_join(threads[i], NULL);
}
