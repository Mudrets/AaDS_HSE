#include <iostream>

/**
 * Выделяет из исходного массива под массив
 * @param arr исходный массив
 * @param count количество элементов
 * @return указатель на первый элемент подмассива
 */
int* subarr(int* arr, int count) {
    int* result = new int[count];
    for (int i = 0; i < count; ++i)
        result[i] = arr[i];
    return result;
}

/**
 * Находит элемент в массиве с помощью бинарного поиска
 * @param vec указатель на начало масива
 * @param size размер массива
 * @param elem искомый элемент
 * @return True - если элемент найден и False в ином случае
 */
int binary_search(std::vector<int> vec, int size, int elem) {
    int left = 0;
    int right = size - 1;
    while (left < right) {
        int middle = (left + right) / 2;
        if (elem >= vec[middle])
            left = middle + 1;
        else
            right = middle - 1;
    }
    if (vec[left] <= elem)
        left++;
    return left;
}

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
        int* sortedPart = subarr(arr, i);
        int j = binary_search(sortedPart, i, val);

        for (int k = i; k > j; --k) {
            arr[k] = arr[k - 1];
            counter++;
        }
        arr[j] = val;

        delete[] sortedPart;
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
