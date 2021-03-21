#include "ReadWriter.h"

int recursiveEntrances = 0;

void quickSort(int* arr, int in, int ik) {
    int i = in;
    int j = ik;
    int pivot = arr[(i + j) / 2];

    while (i <= j) {
        while (i <= j && arr[i] < pivot)
            i++;

        while (j >= i && arr[j] > pivot)
            j--;

        if (i <= j && arr[i] >= pivot && arr[j] <= pivot)
            std::swap(arr[i++], arr[j--]);
    }

    if (in < j) {
        quickSort(arr, in, j);
        recursiveEntrances++;
    }
    if (ik > i) {
        quickSort(arr, i, ik);
        recursiveEntrances++;
    }
}

int main() {
    ReadWriter rw;

    int n = rw.readInt();
    int *arr = new int[n];
    rw.readArray(arr, n);

    quickSort(arr, 0, n - 1);

    rw.writeInt(recursiveEntrances);
    rw.writeArray(arr, n);

    return 0;
}
