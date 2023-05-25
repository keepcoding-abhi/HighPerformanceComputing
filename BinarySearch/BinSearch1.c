#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

int doBinarySearch(int, int, int*, int);

int main() {
	//clock_t starting, finishing;
	//double timeUsed;
	int *arr, n, i, key, blockSize, nthreads, tid, start, end, res;
	printf("Enter the size of array:\n");
	scanf("%d", &n);
	arr = (int*)malloc(sizeof(int) * n);
	i = 0;
	
	#pragma omp parallel for private(i)
	for(i= 0 ; i < n ; i++) {
		arr[i] = i;
	}
	
	/*for(i = 0 ; i < n ; i++) {
		printf("%d ", arr[i]);
	}
	
	printf("\n");*/

	printf("Enter the key that you want to search:\n");
	scanf("%d", &key);
	
	printf("Enter the block size:\n");
	scanf("%d", &blockSize);
	
	nthreads = (n / blockSize) + 1;

	omp_set_num_threads(nthreads);

	//starting = clock();
	#pragma omp parallel private(tid, start, end, res)
	{
		tid = omp_get_thread_num();
		start = blockSize * tid;
		if(start < n) {
			if(start + blockSize <= n) {
				end = start + blockSize - 1;
			}
			else {
				end = n -1;
			}
			printf("Thread %d searching from %d to %d for key: %d.\n", tid, start, end, key);
			res = doBinarySearch(start, end, arr, key);
			//printf("%d\t%d\n", tid, res);
			if(res != -1) {
				printf("Key %d found by thread %d at index %d.\n", key, tid, res);
				//finishing = clock();
			}
		}
	}

	//finishing = clock();
	//timeUsed = finishing - starting;

	//printf("Time taken for parallel : %lf.\n", timeUsed);

	/*starting = clock();
	res = doBinarySearch(0, n - 1, arr, key);
	
	printf("Key %d found at index %d using no parallelism.\n", key, res);
			
	finishing = clock();

	timeUsed = finishing - starting;
		
	printf("Time required without parallelization is %lf.\n", timeUsed);*/

	return 0;
}

int doBinarySearch(int first, int last, int *arr, int key) {
	int mid;
	while(first <= last) {
		mid = (first + last) / 2;
		//printf("%d\n", arr[mid]);
		if(arr[mid] == key) {
			return mid;
		}
		else if(arr[mid] > key) {
			last = mid - 1;
		}
		else {
			first = mid + 1;
		}
	}
	return -1;
}
