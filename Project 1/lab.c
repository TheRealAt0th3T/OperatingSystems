/**
 * Name: Amara Tariq
 * Email: amaratariq@u.boisestate.edu
 * Github Username: TheRealAt0th3t
 */
#include "lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>


/*
 * insertion_sort(int A[], int p, int r):
 *
 * description: Sort the section of the array A[p..r].
 */
void insertion_sort(int A[], int p, int r)
{
	int j;

	for (j=p+1; j<=r; j++) {
		int key = A[j];
		int i = j-1;
		while ((i > p-1) && (A[i] > key)) {
			A[i+1] = A[i];
			i--;
		}
		A[i+1] = key;
	}
}


/*
 * serial_mergesort(int A[], int p, int r):
 *
 * description: Sort the section of the array A[p..r].
 */
void serial_mergesort(int A[], int p, int r)
{
	if (r-p+1 <= INSERTION_SORT_THRESHOLD)  {
			insertion_sort(A,p,r);
	} else {
		int q =  r + (p - r)/2;
		serial_mergesort(A, p, q);
		serial_mergesort(A, q+1, r);
		merge(A, p, q, r);
	}
}

/*
 * merge(int A[], int p, int q, int r):
 *
 * description: Merge two sorted sequences A[p..q] and A[q+1..r]
 *              and place merged output back in array A. Uses extra
 *              space proportional to A[p..r].
 */
void merge(int A[], int p, int q, int r)
{
	int *B = (int *) malloc(sizeof(int) * (r-p+1));

	int i = p;
	int j = q+1;
	int k = 0;
	int l=0;

	// as long as both lists have unexamined elements
	// this loop keeps executing.
	while ((i <= q) && (j <= r)) {
		if (A[i] < A[j]) {
			B[k] = A[i];
			i++;
		} else {
			B[k] = A[j];
			j++;
		}
		k++;
	}

	// now only at most one list has unprocessed elements.

	if (i <= q) {
		// copy remaining elements from the first list
		for (l=i; l<=q; l++) {
			B[k] = A[l];
			k++;
		}
	} else {
		// copy remaining elements from the second list
		for (l=j; l<=r; l++) {
			B[k] = A[l];
			k++;
		}
	}

	// copy merged output from array B back to array A
	k=0;
	for (l=p; l<=r; l++) {
		A[l] = B[k];
		k++;
	}

	free(B);
}

/* 
 * Creating a struct for all the parts of a thread
 *  	- array (A)
 * 		- beginning of array (p)
 * 		- end of array (r)
 */
 struct thread{
	 int* A;
	 int p;
	 int r;
 };

/**
 * void* sorted_thread(void* s)
 * Constructor for the sorted subthreads
 * does this by pointing at its original values and sending it to serial_mergesort
 */
void* sorted_thread(void* st){
	//casting each as a struct pointer and pointing its values in order to store in struct
	int* A = ((struct thread*)st) -> A; 
	int p = ((struct thread*)st)-> p;
	int r = ((struct thread*)st) -> r;

	serial_mergesort(A, p, r);	//sorting the thread 
	return NULL; 
}

/**
 * parallel_mergesort(int A[], int p, int r, int threadNum)
 * sorts array A using multiple threads.
 */
void parallel_mergesort(int A[], int p, int r, int threadNum){

	//local variables used to manipulate the array as we create, split, and join thread processes
	int beg = 0; //p
	int end = 0; //r
	int q = 0; //q for merge()
	int i = 0; //for all the for-loops
	i=p;
	int subArrayLength = r/threadNum; //dividing main array into sub arrays for each thread to have same # of elements

	//Malloc to efficiently set memory aside for our structs for all the threads
		//pointing and taking the size of each struct * the number of threads
	struct thread** tStruct = (struct thread**)malloc(sizeof(struct thread*) * threadNum);	

	//now malloc for the pthread identifers (pthread_t)
	pthread_t* ptID = (pthread_t*)malloc(sizeof(pthread_t) * threadNum);

	//struct for subarray of each thread
	struct thread* sArr;

	//loops through for each thread count
	//splits the array evenly between the threadNum
		//expl A = [2 8 5 3 6 9 1 4]
			// 2 8 -> p=2 r =8
			// 5 3
			// 6 9
			// 1 4
	for(i = 0; i < threadNum; i++){

		//Setting the p and r for each subarray
		beg = i * subArrayLength;
		end = beg + subArrayLength - 1; 

		//Making sure that the last subarray end is correct 
		// if last thread && still more elements? add to the 'end'
		// if end is greater than r? then set back down to r
		if((i == threadNum -1 && end < r) || end > r){
			end = r;
		}

		//Creating subarray for each thread
		sArr = (struct thread*)malloc(sizeof(struct thread));
		sArr -> A = A; //going to be the same for each thread
		sArr -> p = beg;
		sArr -> r = end;

		

		tStruct[i] = sArr; //keeping each subarray struct together for later merging
		
		//Creating each thread
			//(address of thread, NULL, constructor to create, array struct)
		pthread_create(&ptID[i],NULL, sorted_thread, (void*)tStruct[i]); 
	}

	// Join the threads after each thread is done processing the array elements.
	//expl A = [2 8 5 3 6 9 1 4]
			// 2 8 | 3 5 | 6 9 | 1 4
	for (i = 0; i < threadNum; i++){
		pthread_join(ptID[i], NULL);
	}

	//Now to merge together all the threads' arrays
	for(i = 0; i< threadNum-1; i++){
		//finding q so we can merge together all the arrays
		end = tStruct[i+1] -> r; //getting the next subarray's r value
		q = tStruct[i] -> r;	//getting the current subarray's r value

		merge(A, 1, q, end);

		//free memory for each array as we finish merging them together
		free(tStruct[i]);
	}

	//free the remaining memory
	free(tStruct);
	free(sArr);
	free(ptID);
}
