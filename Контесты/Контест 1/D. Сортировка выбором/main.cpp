#include <iostream>

/**
 * Сортирует массив методом выбора минимума
 * @param arr указатель на первый элемент массива
 * @param size размер массива
 * @return количество обменов
 */
int select_sort(int* arr, int size) {
    int counter = 0;
    for (int i = 0; i < size; ++i) {
        int minIndex = i;
        int min = arr[i];
        for (int j = i; j < size; ++j) {
            if (arr[j] < min) {
                min = arr[j];
                minIndex = j;
            }
        }
        if (min != arr[i]) {
            counter++;
            arr[minIndex] = arr[i];
            arr[i] = min;
        }
    }
    return counter;
}

int main() {
    int n; //Размер массива
    std::cin >> n;

    int* arr = new int[n];
    for (int i = 0; i < n; ++i)
        std::cin >> arr[i];

    int count = select_sort(arr, n);
    for (int i = 0; i < n; ++i)
        std::cout << arr[i] << " ";
    std::cout << std::endl << count;

    delete[] arr;
    return 0;
}
