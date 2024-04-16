#include "syscall.h"
int charArrayLength(const char* charArray) {
    int length = 0;
    while (charArray[length] != '\0') {
        length++;
    }
    return length;
}

void numberToCharArray(int number, char* charArray) {

    char tempArray[256]; 
    int i = 0;
    int index = 0;
    while (number != 0) {
        tempArray[index++] = '0' + (number % 10);
        number /= 10;
    }

    // Reverse the temporary array to get the digits in correct order
    for (i = 0; i < index; ++i) {
        charArray[i] = tempArray[index - i - 1];
    }
    charArray[index] = '\0';
}

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

// Partition function to arrange elements around pivot
int partition(int arr[], int low, int high) {
    int pivot = arr[high];  // Choosing the last element as the pivot
    int i = (low - 1);      // Index of smaller element
    int j;
    for (j = low; j <= high - 1; j++) {
        // If current element is smaller than or equal to pivot
        if (arr[j] <= pivot) {
            i++;    // Increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int arr[], int low, int high) 
{
    int pi;
    if (low < high) {
        // pi is partitioning index, arr[p] is now at right place
        pi = partition(arr, low, high);

        // Separately sort elements before partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}


int main()
{
    int arr[7];
    int n = 7;
    char num[255]; //char array to store a number
    int i = 0;
    char endline[2];
    OpenFileId file;

    endline[0] = '\n';
    for (i = 0; i < n; ++i) 
    {
        PrintString("Nhap so nguyen:");
        arr[i] = ReadInt();
    }
    quickSort(arr, 0, n - 1);
    
    Create("quicksort.txt");
    file = Open("quicksort.txt", 0);
    PrintString("Mang int :\n");
    for (i = 0; i < n; i++)
    {
        numberToCharArray(arr[i], num);
        PrintString(num);
        PrintChar('\n');
        Write(num, charArrayLength(num), file);
        Write(endline, 1, file);
    }
    Close(file);
    Halt();
}