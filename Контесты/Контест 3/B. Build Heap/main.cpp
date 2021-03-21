#include "ReadWriter.h"

int left(int i) {
    return 2 * i + 1;
}

int right(int i) {
    return 2 * i + 2;
}

bool isLeaf(int i, int size) {
    return left(i) >= size;
}

int heapfy(int *arr, int i, int size) {
    int l = left(i);
    int r = right(i);
    int largest = l < size && arr[l] > arr[i] ? l : i;
    largest = r < size && arr[r] > arr[largest] ? r : largest;

    int deep = 0;
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        if (!isLeaf(largest, size))
            deep = 1 + heapfy(arr, largest, size);
    }
    return deep;
}

int createHeap(int *arr, int size) {
    int maxDeep = 0;
    for (int i = size - 1; i >= 0; --i) {
        int deep = heapfy(arr, i, size);
        if (deep > maxDeep) maxDeep = deep;
    }
    return maxDeep;
}

int main() {
    ReadWriter rw;

    int n = rw.readInt();
    int *arr = new int[n];
    rw.readArray(arr, n);

    int maxDeep = createHeap(arr, n);

    rw.writeInt(maxDeep);
    rw.writeArray(arr, n);
    return 0;
}