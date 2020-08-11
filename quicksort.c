#include "quicksort.h"
#include<stdio.h>

void swap(int* a, int* b);

int partition (int arr[], int left, int right);

void quickSort(int arr[], int left, int right);

/**
 * A simple function to swap integer pointers.
 * @param a first pointer.
 * @param b second pointer.
 */
void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

/**
 * This function considers arr[right] as the pivot element, and sorts the subarray
 * arr[left],arr[left+1],...,arr[right] s.t. the pivot is placed in its right place,
 * and the rest of the elements are placed to the left of the pivot iff they are smaller.
 * @param arr the complete array.
 * @param left the first index in the subarray (inclusive).
 * @param right the last index in the subarray (inclusive).
 * @return the correct index where the pivot should be placed.
 */
int partition (int arr[], int left, int right)
{
    int pivot = arr[right];
    int smallerElementIndex = (left - 1);
    int i;

    for (i = left; i <= right - 1; ++i)
    {
        if (arr[i] < pivot)
        {
            ++smallerElementIndex;
            swap(&arr[smallerElementIndex], &arr[i]);
        }
    }
    swap(&arr[smallerElementIndex + 1], &arr[right]);
    return (smallerElementIndex + 1);
}

/**
 * Sorts the subarray arr[left],arr[left+1],...,arr[right].
 * @param arr the complete array.
 * @param left the first index in the subarray (inclusive).
 * @param right the last index in the subarray (inclusive).
 */
void quickSort(int arr[], int left, int right)
{
    if (left < right)
    {
        int i = partition(arr, left, right);
        quickSort(arr, left, i - 1);
        quickSort(arr, i + 1, right);
    }
}

/**
 * A simpler way to call quicksort.
 * @param arr an integer array.
 * @param length the length of the array (how many items it contains).
 */
void quickSortArray(int arr[], int length){
    quickSort(arr, 0, length-1);
    return;
}
