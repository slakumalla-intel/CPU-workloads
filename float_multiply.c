
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>


#define MSIZE 100000
#define NUM_CORES 192

float A[MSIZE][MSIZE] = {};
float B[MSIZE][MSIZE] = {};
float C[MSIZE][MSIZE]; 

void initRandomMatrix(float matrix[MSIZE][MSIZE]) {
    for (int i = 0; i < MSIZE; i++) {
        for (int j = 0; j < MSIZE; j++) {
            
            matrix[i][j] = (float)rand();
        }
    }
}


int InitMatrix(float matrix[MSIZE][MSIZE]) {

    
    srand(time(NULL));

    
    initRandomMatrix(matrix);

#ifdef DEBUG
    
    for (int i = 0; i < MSIZE; i++) {
        for (int j = 0; j < MSIZE; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
#endif

    return 0;
}

int coreid = 0;


void *multiplyRowByColumn(void *arg) {

	int row = *(int *)arg;


	cpu_set_t cpuset;
	cpu_set_t mask;
	CPU_ZERO(&cpuset);
	sched_getaffinity(0, sizeof(cpuset), &cpuset);
	int core_count =  CPU_COUNT_S(sizeof(cpuset), &cpuset);
	int affnity_core = row%core_count;

	if( coreid != -1)
	{
		affnity_core =  coreid;
	}
#ifdef CPU_SET_TEST
	// find out if the next core available is in idle state
	if( !CPU_ISSET(affnity_core, &cpuset)) {
		CPU_ZERO(&mask);
		CPU_SET(affnity_core, &mask);
		sched_setaffinity(0, sizeof(mask), &mask);
	}
#endif
		CPU_ZERO(&mask);
		CPU_SET(affnity_core, &mask);
		sched_setaffinity(0, sizeof(mask), &mask);
	//printf(" the core affintted=%d\n", affnity_core);

	for (int i = 0; i < MSIZE; i++) {
		C[row][i] = 0;
		for (int j = 0; j < MSIZE; j++) {
			C[row][i] += A[row][j] * B[j][i];
		}
	}
	pthread_exit(0);
}

int main(int argc, char *argv[]) {
    pthread_t threads[MSIZE];
    int rowIndices[MSIZE]; 

    if ( argc < 2)
    {
	    printf("Usage, ./float_multiply <coreid/-1 for all> \n");
	    exit(1);
    }
    int counter = 0;
    coreid = atoi(argv[1]);
    printf("The core id neterid: %d\n", coreid);
    while(counter < 1)
    {
	InitMatrix(A);
	InitMatrix(B);
    
    for (int i = 0; i < MSIZE; i++) {
        rowIndices[i] = i;
        pthread_create(&threads[i], NULL, multiplyRowByColumn, &rowIndices[i]);
    }

    // Join threads
    for (int i = 0; i < MSIZE; i++) {
        pthread_join(threads[i], NULL);
    }

#ifdef DEBUG
    
    printf("Result Matrix:\n");
    for (int i = 0; i < MSIZE; i++) {
        for (int j = 0; j < MSIZE; j++) {
            printf("%.2f\t", C[i][j]);
        }
        printf("\n");
    }
#endif
    counter++;
    }

    return 0;
}

