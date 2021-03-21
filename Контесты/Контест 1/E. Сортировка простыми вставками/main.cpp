#include <iostream>

/**
 * Сортировка вставками
 * @param arr указатель на первый элемент массива
 * @param size размер массива
 * @return количество элементов, которые уже находились на своем месте
 */
int insertion_sort(int* arr, int size) {
    int counter = 0;
    for (int i = 1; i < size; ++i) {
        int val = arr[i];
        int j = i;
        while (j > 0 && arr[j - 1] > val) {
            arr[j] = arr[j - 1];
            j--;
        }
        if (j == i)
            counter++;
        else
            arr[j] = val;
    }
    return counter;
}

int main() {
    int n; //Размер массива
    std::cin >> n;

    int* arr = new int[n];
    for (int i = 0; i < n; ++i)
        std::cin >> arr[i];

    int count = insertion_sort(arr, n);
    for (int i = 0; i < n; ++i)
        std::cout << arr[i] << " ";
    std::cout << std::endl << count;

    delete[] arr;
    return 0;
}
