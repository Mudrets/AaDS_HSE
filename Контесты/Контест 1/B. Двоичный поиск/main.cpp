#include <iostream>

/**
 * Разворачивает массив целых чисел
 * @param arr указатель на начало массива
 * @param arrSize размер массива
 */
void reverse(int* arr, int arrSize) {
    for (int i = 0; i < arrSize / 2; ++i) {
        int t = arr[i];
        arr[i] = arr[arrSize - i - 1];
        arr[arrSize - i - 1] = t;
    }
}

/**
 * Находит элемент в массиве с помощью бинарного поиска
 * @param arr указатель на начало масива
 * @param size размер массива
 * @param elem искомый элемент
 * @return True - если элемент найден и False в ином случае
 */
bool binary_search(int* arr, int size, int elem) {
    int left = 0;
    int right = size - 1;
    while (left <= right) {
        int middle = (left + right) / 2;
        if (arr[middle] == elem)
            return true;
        if (elem > arr[middle])
            left = middle + 1;
        else
            right = middle - 1;
    }
    return false;
}

int main() {
    int n, k;
    std::cin >> n >> k;

    int *A = new int[n];
    int *B = new int[k];
    for (int i = 0; i < n; ++i)
        std::cin >> A[i];
    for (int i = 0; i < k; ++i)
        std::cin >> B[i];

    reverse(A, n);
    for (int i = 0; i < k; ++i)
        std::cout << (binary_search(A, n, B[i]) ? "YES" : "NO") << std::endl;

    delete[] A;
    delete[] B;
}
