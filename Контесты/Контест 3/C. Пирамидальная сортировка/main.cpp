#include "ReadWriter.h"
//iostream, fstream включены в ReadWriter.h
using namespace std;

void createSortTree(int index, int *arr, int size) {
    int greatestChildIndex;
    while (index * 2 + 1 < size) {
        greatestChildIndex = index * 2 + 1;
        if (index * 2 + 2 < size && arr[greatestChildIndex] < arr[index * 2 + 2])
            greatestChildIndex = index * 2 + 2;
        if (arr[index] >= arr[greatestChildIndex])
            break;
        swap(arr[index], arr[greatestChildIndex]);
        index = greatestChildIndex;
    }
}

//Функция сортировки на куче
//Необходимо реализовать данную функцию.
//Результат должен быть в массиве numbers.
void heapSort(int *numbers, int array_size)
{
    for (int i = array_size - 1; i >= 0; --i)
        createSortTree(i, numbers, array_size);

    for (int i = array_size - 1; i >= 0; --i) {
        swap(numbers[0], numbers[i]);
        createSortTree(0, numbers, i);
    }
}

//Не удалять и не изменять метод main без крайней необходимости.
//Необходимо добавить комментарии, если все же пришлось изменить метод main.
int main()
{
    //Объект для работы с файлами
    ReadWriter rw;

    int *brr = nullptr;
    int n;

    //Ввод из файла
    n = rw.readInt();

    brr = new int[n];
    rw.readArray(brr, n);

    //Запуск сортировки, ответ в том же массиве (brr)
    heapSort(brr, n);

    //Запись в файл
    rw.writeArray(brr, n);

    //освобождаем память
    delete[] brr;

    return 0;
}