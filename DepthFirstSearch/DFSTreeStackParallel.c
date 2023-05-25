#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>

struct node {
	int key;
	struct node *left, *right;
};

struct queueNode {
	struct node *key;
	struct queueNode *next;
};

struct queueNode* delStack(struct queueNode**);
struct queueNode* insertStack(struct queueNode*, struct node*); 
struct node* insert(struct node*, struct queueNode*, int);
void search(struct node*, int);
void delete(struct node*);
void enqueue(struct queueNode*, struct node*);
void preorder(struct node*);

int found;

int main() {
	int n, key;
	do {
		printf("Enter the number of nodes that you want to insert:\n");
		scanf("%d", &n);
	}while(n < 1);
	struct node *root = NULL, *root1, *root2, *root3, *root4;
	struct queueNode *newNode, *head = NULL;
	head = (struct queueNode*)malloc(sizeof(struct queueNode));
	root = (struct node*)malloc(sizeof(struct node));
	root->key = n--;
	head->key = root;
	newNode = (struct queueNode*)malloc(sizeof(struct queueNode));
	newNode->key = NULL;
	newNode->next = NULL;
	head->next = newNode;
	root = insert(root, head, n);
	printf("Enter the key that you want to search:\n");
	scanf("%d", &key);
	clock_t start, end;
	found = -1;
	start = clock();
	if(root->key == key) {
		found = 1;
	}
	root1 = root->left;
	root3 = root->right;
	if(root1->key == key) {
		found = 1;
	}
	else if(root3->key == key) {
		found = 1;
	}
	root2 = root1->right;
	root1 = root1->left;
	root4 = root3->right;
	root3 = root3->left;
	#pragma omp parallel sections num_threads(4) shared(found)
	{
		#pragma omp section
		{
			if(found != 1 && root1 != NULL)
				search(root1, key);
		}
		#pragma omp section
		{
			if(found != 1 && root2 != NULL)
				search(root2, key);
		}
		#pragma omp section
		{
			if(found != 1 && root3 != NULL)
				search(root3, key);
		}
		#pragma omp section
		{
			if(found != 1 && root4 != NULL)
				search(root4, key);
		}
	}
	end = clock();
	printf("Search result for key %d in the tree : %s.\n", key, (found == 1 ? "true" : "false"));
	delete(root);
	double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Time taken %lf.\n", cpu_time);
	return 0;
}

void preorder(struct node *current) {
	if(current == NULL)
		return;
	printf("%d ", current->key);
	preorder(current->left);
	preorder(current->right);
}

void displayQueue(struct queueNode *head) {
	struct queueNode *current = head;
	while(current != NULL) {
		if(current->key == NULL) {
			printf("NULL ");
		}
		else 
			printf("%d ", current->key->key);
		current = current->next;
	}
	printf("\n");
}

void delete(struct node *current) {
	if(current == NULL)
		return;
	delete(current->left);
	delete(current->right);
	free(current);
}

void search(struct node *root, int key) {
	struct node *current = root;
	if(root == NULL)
		return;
	struct queueNode *popped, *top;
	top = NULL;	
	while(current != NULL) {
		if(found == 1)
			break;
		printf("Element : %d, checked from thread %d.\n", current->key, omp_get_thread_num());
		if(current->key == key) {
			found = 1;
			break;
		}
		top = insertStack(top, current);
		current = current->left;
	}
	while((popped = delStack(&top)) != NULL) {
		if(found != 1) {		
			current = popped->key;
			current = current->right;
			while(current != NULL) {
				printf("Element : %d, checked from thread %d.\n", current->key, omp_get_thread_num());
				if(current->key == key) {
					found = 1;
					break;
				}
				top = insertStack(top, current);
				current = current->left;
			}
		}
		free(popped);
	}
}

struct queueNode* delStack(struct queueNode **top) {
	if(*top == NULL)
		return NULL;
	struct queueNode *toReturn = *top;
	*top = (*top)->next;
	return toReturn;
}

struct queueNode* insertStack(struct queueNode *head, struct node *newNode) {
	struct queueNode *toAdd = (struct queueNode*)malloc(sizeof(struct queueNode));
	toAdd->key = newNode;
	if(head == NULL) {
		toAdd->next = NULL;
	}
	else{
		toAdd->next = head;		
	}
	return toAdd;
}

struct node* insert(struct node *root, struct queueNode *head, int n) {
	struct queueNode *current;
	struct node *currentTree, *newTree;
	int i;
	while(1) {
		current = head;
		head = head->next;
		currentTree = current->key;
		if(currentTree == NULL) {
			if(current->next == NULL) {
				printf("Queue Empty.\n");
				break;
			}
			enqueue(head, NULL);
		}
		else {
			currentTree->left = NULL;
			currentTree->right = NULL;
			for(i = 0 ; i < 2 ; i++) {
				if(n > -1) {
					newTree = (struct node*)malloc(sizeof(struct node));
					newTree->key = n--;
					newTree->left = newTree->right = NULL;
					if(i == 0)
						currentTree->left = newTree;
					else
						currentTree->right = newTree;
					enqueue(head, newTree);
				}
				else
					break;
			}
			if(n < 0)
				break;
		}
		free(current);
	}
	while(head != NULL) {
		current = head;
		head = head->next;
		free(current);
	}
	return root;
}

void enqueue(struct queueNode *head, struct node *newNode) {
	struct queueNode *current, *new1 = (struct queueNode*)malloc(sizeof(struct queueNode));
	new1->key = newNode;
	new1->next = NULL;
	current = head;
	while(current->next != NULL)
		current = current->next;
	current->next = new1;
}
