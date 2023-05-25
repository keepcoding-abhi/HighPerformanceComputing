#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "mpi.h"


#define ARRAY_SIZE 1
#define ARRAY_TO_SORT 2
#define SORTED_ARRAY 3

void parallelMerge(int*, int, int, int, int, int, int);
void merge(int*, int, int, int);
void display(int*, int);
int IncOrder(const void*, const void*);

int main(int argc, char **argv) {
	int myrank, nProc, *arr, nlocal, i;
	char *size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nProc);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	if(myrank == 0) {
		double start, end;
		start = MPI_Wtime();
		int rootHeight;
		if(argc > 1) {
			i = 0;
			nlocal = 0;
			size = argv[1];
			while(size[i] != '\0') {
				nlocal = nlocal * 10 + (size[i++] - '0');
			}
		}
		else {
			nlocal = 100;
		}
		arr = (int*) malloc(sizeof(int) * nlocal);
		for(i = 0 ; i < nlocal ; i++) {
			arr[i] = nlocal - i;
		}
		rootHeight = 0;
		i = 1;
		while(i < nProc) {
			i += i;
			rootHeight++;
		}
		printf("%d processes allows root height of %d.\n", nProc, rootHeight);
		printf("Array to be sorted :\n");
		display(arr, nlocal);
		//clock_t start, end;
		//start = clock();
		parallelMerge(arr, 0, nlocal - 1, 0, rootHeight, 0, nProc);
		//end = clock();
		printf("Sorted array :\n");
		display(arr, nlocal);
		MPI_Finalize();
		end = MPI_Wtime();
		printf("Time taken : %lf seconds.\n", end - start);
		//double cpu_time_used = double((end - start)) / CLOCKS_PER_SEC;
		//printf("Time required : %lf.\n", cpu_time_used);
	}
	else {
		arr = (int*)malloc(sizeof(int) * 2);
		MPI_Status status;
		MPI_Recv(arr, 2, MPI_INT, MPI_ANY_SOURCE, ARRAY_SIZE, MPI_COMM_WORLD, &status);
		nlocal = arr[0];
		int myHeight = arr[1];
		int myParent = status.MPI_SOURCE;
		arr = (int*)malloc(sizeof(int) * nlocal);
		MPI_Recv(arr, nlocal, MPI_INT, myParent, ARRAY_TO_SORT, MPI_COMM_WORLD, &status);
		parallelMerge(arr, 0, nlocal - 1, myrank, myHeight, myParent, nProc);
		MPI_Finalize();
	}
	return 0;
}

void parallelMerge(int *arr, int low, int high, int myRank, int myHeight, int parent, int nProc) {
	if(low < high) {
		if(myHeight > 0) {
			int mid = (low + high) / 2;
			int rightChild = myRank | (1 << (myHeight - 1));
			if(rightChild >= nProc) {
				parallelMerge(arr, low, high, myRank, myHeight - 1, myRank, nProc);
			}
			else{
				int *toSend = (int*)malloc(sizeof(int) * 2);
				toSend[0] = high - mid;
				toSend[1] = myHeight - 1;
				MPI_Send(toSend, 2, MPI_INT, rightChild, ARRAY_SIZE, MPI_COMM_WORLD);
				MPI_Send(arr + mid + 1, high - mid, MPI_INT, rightChild, ARRAY_TO_SORT, MPI_COMM_WORLD);
				parallelMerge(arr, low, mid, myRank, myHeight - 1, myRank, nProc); 
				MPI_Status status;
				MPI_Recv(arr + mid + 1, high - mid, MPI_INT, rightChild, SORTED_ARRAY, MPI_COMM_WORLD, &status);
				merge(arr, low, mid, high);
			}
		}
		else {
			qsort(arr + low, high - low + 1, sizeof(int), IncOrder);
		}
		if(parent != myRank) {
			MPI_Send(arr, high - low + 1, MPI_INT, parent, SORTED_ARRAY, MPI_COMM_WORLD);
		}
	}
}

void merge(int *arr, int low, int mid, int high) {
	int *left, *right, nleft, nright, i, j, k;
	nleft = mid - low + 1;
	nright = high - mid;
	left = (int*)malloc(sizeof(int) * nleft);
	right = (int*)malloc(sizeof(int) * nright);
	for(i = 0 ; i < nleft ; i++) {
		left[i] = arr[low + i];
	}
	for(i = 0 ; i < nright ; i++) {
		right[i] = arr[mid + 1 + i];
	}

	k = low;
	i = j = 0;
	while(i < nleft && j < nright) {
		if(left[i] > right[j]) {
			arr[k] = right[j++];
		}
		else {
			arr[k] = left[i++];
		}
		k++;
	}

	while(j < nright) {
		arr[k++] = right[j++];
	}
	while(i < nleft) {
		arr[k++] = left[i++];
	}

	free(left);
	free(right);
}

void display(int *arr, int num) {
	int i;
	for(i = 0 ; i < num - 1 ; i++) {
		printf("%d ", arr[i]);
	}
	printf("%d.\n", arr[i]);
}

int IncOrder(const void *e1, const void *e2) {
	return (*((int *)e1) - *((int *)e2));
}

