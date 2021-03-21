#include "ReadWriter.h"
//iostream, fstream включены в ReadWriter.h
using namespace std;

// Функция сортировки подсчетом
void countingSort(int *numbers, int array_size)
{
    //Находим максимально возможное количество различных чисел
    //в массиве по его минимуму и максимуму
    int min = 1000000, max = 0;
    for(int i = 0; i < array_size; ++i)
    {
        min = numbers[i] < min ? numbers[i] : min;
        max = numbers[i] > max ? numbers[i] : max;
    }

    int counterSize = (max - min) + 1;
    int* counters = new int[counterSize];
    for (int i = 0; i < counterSize; ++i)
        counters[i] = 0; //Зануляем массив

    //Подсчитываем количество непревышающих элементов для
    //каждого числа исходного массива
    for (int i = 0; i < array_size; ++i)
        ++counters[numbers[i] - min];
    for (int i = 1; i < counterSize; ++i)
        counters[i] += counters[i - 1];

    int* result = new int[array_size]; //Отсортированный массив
    for (int i = array_size - 1; i >= 0; --i)
        result[--counters[numbers[i] - min]] = numbers[i]; //Сортируем массив

    for (int i = 0; i < array_size; ++i)
        numbers[i] = result[i]; //Копируем отсортированный массив в исходный

    //Освобождаем память
    delete[] counters;
    delete[] result;
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
    countingSort(brr, n);

    //Запись в файл
    rw.writeArray(brr, n);

    //освобождаем память
    delete[] brr;

    return 0;
}
