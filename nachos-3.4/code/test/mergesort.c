#include "syscall.h"
int charArrayLength(const char* charArray) {
    int length = 0;
    while (charArray[length] != '\0') {
        length++;
    }
    return length;
}

void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temporary arrays
    int L[n1], R[n2];
    int i;
    int j;
    int k;
    // Copy data to temporary arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Merge the temporary arrays back into arr[l..r]
     i = 0; // Initial index of first subarray
     j = 0; // Initial index of second subarray
     k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(int arr[], int l, int r) {
    int m;
    if (l >= r)
        return; // Returns recursively when the array is divided into single element

    m = l + (r - l) / 2;
    mergeSort(arr, l, m);     // Sorts the first half
    mergeSort(arr, m + 1, r); // Sorts the second half
    merge(arr, l, m, r);      // Merges the sorted halves
}


int main()
{
    int arr[5];
    int n = 5;
    char num[255]; //char array to store a number
    int i = 0;
    char endline[2];
    int buffersize;
    OpenFileId file;

    endline[0] = '\n';
    for (i = 0; i < n; ++i) 
    {
        PrintString("Nhap so thuc:");
        arr[i] = ReadFloat();
    }
    mergeSort(arr, 0, n - 1);
    
    Create("mergesort.txt");
    file = Open("mergesort.txt", 0);

    for (i = 0; i < n; i++)
    {
        buffersize = FloatToBuffer(num, arr[i]);
        num[buffersize] = ' ';
        buffersize++;
        num[buffersize] = '\0';
        Write(num, buffersize, file);
    }

    Close(file);
    Halt();
}