#include "ReadWriter.h"
#include <iostream>

int findLastElemBinHeap(int *arr, int size) {
    for(int i = 1; i < size; ++i) {
        int parentIndex; //Индекс родителя
        //Находим родителя
        if (i % 2 == 0)
            parentIndex = (i - 2) / 2;
        else
            parentIndex = (i - 1) / 2;

        if (arr[i] > arr[parentIndex])
            return i - 1;
    }
    return size - 1;
}

int main() {
    ReadWriter rw;

    int n = rw.readInt();
    int *arr = new int[n];
    rw.readArray(arr, n);

    int lastIndex = findLastElemBinHeap(arr, n);

    rw.writeInt(lastIndex);

    delete[] arr;

    return 0;
}