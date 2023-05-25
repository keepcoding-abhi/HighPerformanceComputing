#include<stdio.h>
#include<stdlib.h>
#include<time.h>


void display(int*, int);
void mergeSort(int*, int, int);
void merge(int*, int, int, int);

int main(int argc, char **argv) {
	int nlocal, i, *arr;
	clock_t start, end;
	start = clock();

	if(argc > 1) {
		nlocal = 0;
		char *size = argv[1];
		i = 0;
		while(size[i] != '\0') {
			nlocal = (nlocal * 10) + (size[i++] - '0');
		}
	}
	else {
		nlocal = 100;
	}
	
	arr = (int*)malloc(sizeof(int) * nlocal);
	for(i = 0 ; i < nlocal ; i++) {
		arr[i] = nlocal - i;
	}
	printf("Elements to be sorted :\n");
	display(arr, nlocal);

	mergeSort(arr, 0, nlocal - 1);

	end = clock();	
	printf("Sorted array :\n");
	display(arr, nlocal);
	end = clock();
	printf("Time taken = %lf seconds.\n", ((double)(end - start)) / CLOCKS_PER_SEC);
	
	return 0;
}

void mergeSort(int *arr, int low, int high) {
	if(low < high) {
		int mid = (low + high) / 2;
		mergeSort(arr, low, mid);
		mergeSort(arr, mid + 1, high);
		merge(arr, low, mid,high);
	}
}

void merge(int *arr, int low, int mid, int high) {
	int *left, *right, i, j, k, nleft, nright;
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

	i = j = 0;
	k = low;

	while(i < nleft && j < nright) {
		if(left[i] > right[j]) {
			arr[k] = right[j++];
		}
		else {
			arr[k] = left[i++];
		}
		k++;
	}

	while(i < nleft) {
		arr[k++] = left[i++];
	}

	while(j < nright) {
		arr[k++] = right[j++];
	}
	free(left);
	free(right);
}

void display(int *arr, int num) {
	int i;
	for(i = 0 ; i < num - 1 ; i++) {
		printf("%d ", arr[i]);
	}
	printf("%d.\n", arr[num - 1]);
}
