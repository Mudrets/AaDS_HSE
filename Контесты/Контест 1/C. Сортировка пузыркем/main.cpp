#include <iostream>

/**
 * Сортирует массив
 * @param arr указатель на первый элемент массива
 * @param size размер массива
 * @return количество обменов совершенных при сортировке
 */
int bubble_sort(int* arr, int size) {
    int counter = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                int t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
                counter++;
            }
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

    int count = bubble_sort(arr, n);
    for (int i = 0; i < n; ++i)
        std::cout << arr[i] << " ";
    std::cout << std::endl << count;

    delete[] arr;
    return 0;
}
