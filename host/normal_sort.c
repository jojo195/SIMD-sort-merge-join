#include "../support/types.h"
#include "normal_sort.h"
#include "../support/config.h"
#include <stdlib.h>
//split function
int part_tuple(tuple_t* r, int low, int high)
{
	int i = low, j = high;
    tuple_t pivot = r[low]; //temporery store base element
	while (i < j)
	{
		while (i<j && r[j].key > pivot.key)
		{
			j--;
		}
		if (i < j)
		{
			r[i].key = r[j].key;
            r[i].payload = r[j].payload;
			i++;
			// swap(r[i++], r[j]);
		}
		while (i < j && r[i].key <= pivot.key)
		{
			i++;
		}
		if (i < j)
		{
			r[j].key = r[i].key;
            r[j].payload = r[i].payload;
			j--;
			// swap(r[i], r[j--]);
		}
		r[i].key = pivot.key;
        r[i].payload = pivot.payload;
	}
	return i; //return the position of base element
}
/**
 * @param r array of tuples to be sort
 * @param low lower index of tuple array
*/
void Quicksort_tuple(tuple_t* r, int low, int high)
{
	int mid;
	if (low < high)
	{
		mid = part_tuple(r, low, high);  
		Quicksort_tuple(r, low, mid - 1); //recursive sort of left sub array
		Quicksort_tuple(r, mid+1, high); //recursive sort of right sub array
	}
}
void Quicksort_relation(relation_t* r)
{
    Quicksort_tuple(r->tuples, 0, r->num_tuples - 1);
}

// Recursive implementation of merge sort
// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
//@param tmp_L and tmp_R is demand to be CACHE_LINE aligned
//and exactly the same size as arr
void merge_tuple(tuple_t* arr, tuple_t* tmp_L, tuple_t* tmp_R, int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;


	// Copy data to temp arrays
	// L[] and R[]
	
    memcpy(tmp_L, arr+l, n1);
    memcpy(tmp_R, arr+m+1, n2);
	// Merge the temp arrays back
	// into arr[l..r]
	// Initial index of first subarray
	i = 0;

	// Initial index of second subarray
	j = 0;

	// Initial index of merged subarray
	k = l;
	while (i < n1 && j < n2)
	{
		if (tmp_L[i].key <= tmp_R[j].key)
		{
			arr[k].key = tmp_L[i].key;
            arr[k].payload = tmp_L[i].payload;
			i++;
		}
		else
		{
			arr[k].key = tmp_R[j].key;
            arr[k].payload = tmp_R[j].payload;
			j++;
		}
		k++;
	}

	// Copy the remaining elements
	// of L[], if there are any
	while (i < n1) {
        arr[k].key = tmp_L[i].key;
		arr[k].payload = tmp_L[i].payload;
		i++;
		k++;
	}

	// Copy the remaining elements of
	// R[], if there are any
	while (j < n2)
	{
		arr[k].key = tmp_R[j].key;
        arr[k].payload = tmp_R[j].payload;
		j++;
		k++;
	}
}

// l is for left index and r is
// right index of the sub-array
// of arr to be sorted
void Mergesort_tuple(tuple_t* arr, tuple_t* tmp_L, tuple_t* tmp_R, int l, int r)
{
	if (l < r)
	{
		// Same as (l+r)/2, but avoids
		// overflow for large l and h
		int m = l + (r - l) / 2;

		// Sort first and second halves
		Mergesort_tuple(arr, tmp_L, tmp_R, l, m);
		Mergesort_tuple(arr, tmp_L, tmp_R, m + 1, r);

		merge_tuple(arr, tmp_L, tmp_R, l, m, r);
	}
}


void Mergesort_relation(relation_t* r)
{
	tuple_t* tmp_L = aligned_alloc(r->num_tuples*sizeof(tuple_t)/2, CACHE_LINE_SIZE);
	tuple_t* tmp_R = aligned_alloc(r->num_tuples*sizeof(tuple_t)/2, CACHE_LINE_SIZE);
    Mergesort_tuple(r->tuples, tmp_L, tmp_R, 0, r->num_tuples - 1);
}