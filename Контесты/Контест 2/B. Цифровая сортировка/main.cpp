#include <iostream>
#include "vector"
#include "fstream"

//Для удобного получения разрядов числа в 8-ом представлении
union Un
{
    unsigned int number;
    unsigned char mod256[4];
};

// Функция сортировки подсчетом
void countingSort(Un *numbers, int array_size, int discharge)
{
    //Находим максимально возможное количество различных чисел
    //в массиве по его минимуму и максимуму
    int min = 1000000, max = 0;
    for(int i = 0; i < array_size; ++i)
    {
        min = numbers[i].mod256[discharge] < min ? numbers[i].mod256[discharge] : min;
        max = numbers[i].mod256[discharge] > max ? numbers[i].mod256[discharge] : max;
    }

    int counterSize = (max - min) + 1;
    int* counters = new int[counterSize];
    for (int i = 0; i < counterSize; ++i)
        counters[i] = 0; //Зануляем массив

    //Подсчитываем количество непревышающих элементов для
    //каждого числа исходного массива
    for (int i = 0; i < array_size; ++i)
        ++counters[numbers[i].mod256[discharge] - min];
    for (int i = 1; i < counterSize; ++i)
        counters[i] += counters[i - 1];

    Un* result = new Un[array_size]; //Отсортированный массив
    for (int i = array_size - 1; i >= 0; --i)
        result[--counters[numbers[i].mod256[discharge] - min]] = numbers[i]; //Сортируем массив

    delete[] counters;

    for (int i = 0; i < array_size; ++i)
        numbers[i] = result[i]; //Копируем отсортированный массив в исходный

    //Освобождаем память
    delete[] result;
}

// Функция цифровой сортировки
void radixSort(Un *numbers, int array_size)
{
    int maxDischargeNum = 0; //Максимальное кол-во разрядов числа в 8-ом представлении

    //Находим maxDischargeNum
    for (int i = 0; i < array_size; ++i)
    {
        for (int j = maxDischargeNum; j < 4; ++j)
            if (numbers[i].mod256[j] > 0 && j > maxDischargeNum)
                maxDischargeNum = j;
    }

    for (int i = 0; i < maxDischargeNum + 1; ++i)
        countingSort(numbers, array_size, i);
}

int main()
{
    std::fstream fin, fout;
    fin.open("input.txt", std::ios::in);

    if (!fin.is_open()) {
        throw std::ios_base::failure("file not open");
    }

    int n = 0;
    fin >> n;
    Un *drr = new Un[n];
    for (int i = 0; i < n; ++i)
        fin >> drr[i].number;
    fin.close();

    radixSort(drr, n);

    fout.open("output.txt", std::ios::out);
    if (!fout.is_open())
        throw std::ios_base::failure("file not open");

    for (int i = 0; i < n; i++)
        fout << drr[i].number << " ";
    fout.close();

    delete[] drr;

    return 0;
}
