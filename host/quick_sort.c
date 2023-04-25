#include "../support/types.h"
#include "quick_sort.h"
//split function
int part(int* r, int low, int high)
{
	int i = low, j = high, pivot = r[low]; //temporery store base element
	while (i < j)
	{
		while (i<j && r[j]>pivot)
		{
			j--;
		}
		if (i < j)
		{
			r[i]=r[j];
			i++;
			// swap(r[i++], r[j]);
		}
		while (i < j && r[i] <= pivot)
		{
			i++;
		}
		if (i < j)
		{
			r[j]=r[i];
			j--;
			// swap(r[i], r[j--]);
		}
		r[i]=pivot;
	}
	return i; //return the position of base element
}
void Quicksort(int* r, int low, int high)
{
	int mid;
	if (low < high)
	{
		mid = part(r, low, high);  
		Quicksort(r, low, mid - 1); //recursive sort of left sub array
		Quicksort(r, mid+1, high); //recursive sort of right sub array
	}
}

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