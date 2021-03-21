#include "ReadWriter.h"

int recursiveEntrances = 0;

int lomuto(int* arr, int low, int high) {
    int pivot = arr[high];
    int i = low;
    for (int j = low; j < high; ++j)
        if (arr[j] <= pivot)
            std::swap(arr[i++], arr[j]);

    std::swap(arr[i], arr[high]);
    return i;
}

void quickSort(int* arr, int lo, int hi) {
    if (lo < hi) {
        recursiveEntrances++;
        int p = lomuto(arr, lo, hi);
        if (p > 0)
            quickSort(arr, lo, p - 1);
        quickSort(arr, p + 1, hi);
    }
}

int main() {
    ReadWriter rw;

    int n = rw.readInt();
    int *arr = new int[n];
    rw.readArray(arr, n);

    quickSort(arr, 0, n - 1);

    rw.writeInt(--recursiveEntrances);
    rw.writeArray(arr, n);

    return 0;
}
