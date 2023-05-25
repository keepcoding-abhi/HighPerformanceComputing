#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"

#define ARRAY_SIZE 1
#define ARRAY_TO_SORT 2
#define SORTED_ARRAY 3

void display(int*, int);
int IncOrder(const void*, const void*);
void compareSplit(int*, int*, int*, int, int, int);

int main(int argc, char **argv) {
	int myrank, nlocal, *arr, nProc, *blockSize, i, j, ntotal;
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	double start = MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProc);

	if(myrank == 0) {
		int block;
		if(argc > 1) {
			char *size = argv[1];
			ntotal = i = 0;
			while(size[i] != '\0') {
				ntotal = (ntotal * 10) + (size[i++] - '0');
			}
		}
		else {
			ntotal = 100;
		}
		arr = (int*)malloc(sizeof(int) * ntotal);
		for(i = 0 ; i < ntotal ; i++) {
			arr[i] = ntotal - i;
		}
		printf("Process 0 created :\n");
		display(arr, ntotal);
		if(nProc > 1 && ntotal >= nProc) {
			block = ntotal / nProc;
			nlocal = block;
			i = block;
			blockSize = (int*)malloc(sizeof(int));
			blockSize[0] = block;
			for(j = 1 ; j < nProc - 1 ; j++) {
				MPI_Send(blockSize, 1, MPI_INT, j, ARRAY_SIZE, MPI_COMM_WORLD);
				MPI_Send(arr + i, block, MPI_INT, j, ARRAY_TO_SORT, MPI_COMM_WORLD);
				i += block;
			}
			blockSize[0] = ntotal - i;
			MPI_Send(blockSize, 1, MPI_INT, j, ARRAY_SIZE, MPI_COMM_WORLD);
			MPI_Send(arr + i, blockSize[0], MPI_INT, j, ARRAY_TO_SORT, MPI_COMM_WORLD);
			free(blockSize);
		}
		else {
			if(ntotal < nProc) {
				blockSize = (int *)malloc(sizeof(int));
				blockSize[0] = -1;
				for(j = 1 ; j < nProc ; j++) {
					MPI_Send(blockSize, 1, MPI_INT, j, ARRAY_SIZE, MPI_COMM_WORLD);
				}
				free(blockSize);
			}
			nlocal = ntotal;
			qsort(arr, ntotal, sizeof(int), IncOrder);
			printf("Sorted array :\n");
			display(arr, ntotal);
			double end = MPI_Wtime();
			MPI_Finalize();
			printf("The process took %lf seconds.\n", (end - start));
			return 0;
		}
		printf("Process 0 owns :\n");
		display(arr, nlocal);
	}
	else {
		arr = (int*)malloc(sizeof(int));
		MPI_Recv(arr, 1, MPI_INT, 0, ARRAY_SIZE, MPI_COMM_WORLD, &status);
		nlocal = arr[0];
		free(arr);
		if(nlocal == -1) {
			MPI_Finalize();
			return 0;
		}
		arr = (int*)malloc(sizeof(int) * nlocal);
		MPI_Recv(arr, nlocal, MPI_INT, 0, ARRAY_TO_SORT, MPI_COMM_WORLD, &status);
		printf("Process %d owns :\n", myrank);
		display(arr, nlocal);
	}

	int nrecv, oddrank, evenrank, *wspace, *received, currentRecv;

	nrecv = nlocal;
	if(myrank == nProc - 1) {
		blockSize = (int*)malloc(sizeof(int));
		blockSize[0] = nlocal;
		MPI_Send(blockSize, 1, MPI_INT, nProc - 2, ARRAY_SIZE, MPI_COMM_WORLD);
		free(blockSize);
	}
	if(myrank == nProc - 2) {
		blockSize = (int*)malloc(sizeof(int));
		MPI_Recv(blockSize, 1, MPI_INT, nProc - 1, ARRAY_SIZE, MPI_COMM_WORLD, &status);
		nrecv = blockSize[0];
		free(blockSize);
	}

	if(myrank % 2 == 0) {
		evenrank = myrank + 1;
		oddrank = myrank - 1; 
	}
	else {
		oddrank = myrank + 1;
		evenrank = myrank - 1;
	}

	if(oddrank == -1 || oddrank == nProc) {
		oddrank = MPI_PROC_NULL;
	}
	if(evenrank == -1 || evenrank == nProc) {
		evenrank = MPI_PROC_NULL;
	}

	qsort(arr, nlocal, sizeof(int), IncOrder);

	//printf("Array before odd even sort in process %d:\n", myrank);
	//display(arr, nlocal);

	received = (int*)malloc(sizeof(int) * nrecv);
	wspace = (int*)malloc(sizeof(int) * nlocal);

	for(i = 0 ; i < nProc ; i++) {
		if(i % 2 == 0) {
			if(myrank % 2 == 0) {
				MPI_Send(arr, nlocal, MPI_INT, evenrank, SORTED_ARRAY, MPI_COMM_WORLD);
				MPI_Recv(received, nrecv, MPI_INT, evenrank, SORTED_ARRAY, MPI_COMM_WORLD, &status); 
			}
			else {
				MPI_Recv(received, nrecv, MPI_INT, evenrank, SORTED_ARRAY, MPI_COMM_WORLD, &status);
				MPI_Send(arr, nlocal, MPI_INT, evenrank, SORTED_ARRAY, MPI_COMM_WORLD);
			}
		}
		else {
			if(myrank % 2 == 1) {
				MPI_Send(arr, nlocal, MPI_INT, oddrank, SORTED_ARRAY, MPI_COMM_WORLD);
				MPI_Recv(received, nrecv, MPI_INT, oddrank, SORTED_ARRAY, MPI_COMM_WORLD, &status);
			}
			else {
				MPI_Recv(received, nrecv, MPI_INT, oddrank, SORTED_ARRAY, MPI_COMM_WORLD, &status);
				MPI_Send(arr, nlocal, MPI_INT, oddrank, SORTED_ARRAY, MPI_COMM_WORLD);
			}
		}
		if(status.MPI_SOURCE > -1) {
			MPI_Get_count(&status, MPI_INT, &currentRecv);
			compareSplit(arr, received, wspace, nlocal, currentRecv, myrank > status.MPI_SOURCE);
		}
	}

	free(wspace);
	free(received);

	//printf("Array in process %d :\n", myrank);
	//display(arr, nlocal);

	if(myrank == 0) {
		j = nlocal;
		for(i = 1 ; i < nProc ; i++) {
			MPI_Recv(arr + j, ntotal - j, MPI_INT, i, SORTED_ARRAY, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_INT, &nrecv);
			j += nrecv;
		}
		printf("Sorted array :\n");
		display(arr, ntotal);
		double end = MPI_Wtime();
		printf("The process took %lf seconds.\n", (end - start)); 
	}
	else {
		MPI_Send(arr, nlocal, MPI_INT, 0, SORTED_ARRAY, MPI_COMM_WORLD);
	}
	free(arr);
	MPI_Finalize();

	return 0;
}

void compareSplit(int *arr, int *received, int *wspace, int nlocal, int nrecv, int keepLarge) {
	int i, j, k;

	for(i = 0 ; i < nlocal ; i++) {
		wspace[i] = arr[i];
	}

	if(keepLarge) {
		k = i = nlocal - 1;
		j = nrecv - 1;
		while(k > -1 && i > -1 && j > -1) {
			if(wspace[i] > received[j]) {
				arr[k] = wspace[i--];
			}
			else {
				arr[k] = received[j--];
			}
			k--;
		}

		while(k > -1 && i > -1) {
			arr[k--] = wspace[i--];
		}

		while(k > -1 && j > -1) {
			arr[k--] = received[j--];
		}
	}
	else {
		k = i = j = 0;
		while(k < nlocal && i < nlocal && j < nrecv) {
			if(wspace[i] > received[j]) {
				arr[k] = received[j++];
			}
			else {
				arr[k] = wspace[i++];
			}
			k++;
		}

		while(k < nlocal && i < nlocal) {
			arr[k++] = wspace[i++];
		}

		while(k < nlocal && j < nrecv) {
			arr[k++] = received[j++];
		}
	}
}

int IncOrder(const void *e1, const void *e2) {
	return (*((int*)e1) - *((int*)e2));
}

void display(int *arr, int num) {
	int i;
	for(i = 0 ; i < num - 1 ; i++) {
		printf("%d ", arr[i]);
	}
	printf("%d.\n", arr[i]);
}

