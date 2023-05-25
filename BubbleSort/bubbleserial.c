#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void display(int*, int);

int main(int argc, char **argv) {
	clock_t start, end;
	start = clock();
	int nlocal, i, j, swapped;
	if(argc > 1) {
		char *size = argv[1];
		i = 0;
		nlocal = 0;
		while(size[i] != '\0') {
			nlocal = (nlocal * 10) + (size[i++] - '0');
		}
	}
	else {
		nlocal = 100;
	}
	int *arr = (int*)malloc(sizeof(int) * nlocal);
	for(i = 0 ; i < nlocal ; i++) {
		arr[i] = nlocal - i;
	}
	printf("Elements to be sorted :\n");
	display(arr, nlocal);
	for(i = 1 ; i < nlocal ; i++) {
		swapped = 0;
		for(j = 0 ; j < nlocal - i ; j++) {
			if(arr[j] > arr[j + 1]) {
				swapped = 1;
				arr[j + 1] = arr[j] + arr[j + 1];
				arr[j] = arr[j + 1] - arr[j];
				arr[j + 1] = arr[j + 1] - arr[j];
			}
		}
		if(swapped == 0) {
			break;
		}
	}
	printf("Sorted elements:\n");
	display(arr, nlocal);
	end = clock();
	printf("The process took :%lf seconds.\n", (double)(end - start) / CLOCKS_PER_SEC);
	return 0;
}

void display(int *arr, int num) {
	int i;
	for(i = 0 ; i < num - 1 ; i++) {
		printf("%d ", arr[i]);
	}
	printf("%d.\n", arr[num - 1]);
}
