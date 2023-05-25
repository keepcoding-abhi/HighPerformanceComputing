#include<stdio.h>
#include<stdlib.h>
int doBinarySearch(int, int, int*, int);

int main() {
	int *arr, n, i, key,res;
	printf("Enter the size of array:\n");
	scanf("%d", &n);
	arr = (int*)malloc(sizeof(int) * n);
	i = 0;
	for(i= 0 ; i < n ; i++) {
		arr[i] = i;
	}
	printf("Enter the key that you want to search:\n");
	scanf("%d", &key);
	res = doBinarySearch(0, n - 1, arr, key);
	
	printf("Key %d found at index %d using no parallelism.\n", key, res);
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
