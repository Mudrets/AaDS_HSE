#include "iostream"

class Sortings {
private:
    /**
     * Возводит 10 в степень power
     * @param power степень
     * @return 10^power
     */
    static int powOf10(int power) {
        int res = 1;
        for (int i = 0; i < power; ++i)
            res *= 10;
        return res;
    }

    /**
     * Сортировка подсчетом для цифр
     * @param numbers массив чисел
     * @param arraySize размер массива
     * @param numDigit номер цифры по которой сортируем
     */
    static void countingSortForDigits(int *numbers, int arraySize, int numDigit)
    {
        int* counters = new int[10];
        for (int i = 0; i < 10; ++i)
            counters[i] = 0; //Зануляем массив

        //Подсчитываем количество непревышающих элементов для
        //каждого числа исходного массива
        int pow = powOf10(numDigit - 1);
        for (int i = 0; i < arraySize; ++i)
            ++counters[numbers[i] / pow % 10];

        for (int i = 1; i < 10; ++i)
            counters[i] += counters[i - 1];

        int* result = new int[arraySize]; //Отсортированный массив
        for (int i = arraySize - 1; i >= 0; --i)
            result[--counters[numbers[i] / pow % 10]] = numbers[i]; //Сортируем массив


        for (int i = 0; i < arraySize; ++i)
            numbers[i] = result[i]; //Копируем отсортированный массив в исходный

        //Освобождаем память
        delete[] counters;
        delete[] result;
    }

    /**
     * Слияние отсортированных в порядке неубывания массивов с
     * сортировкой полученного массива в порядке неубывания
     * @param arr сортируемый массив
     * @param firstInd начало первого подмассива
     * @param lastInd конец второго подмассива
     */
    static void merge(int* arr, int firstInd, int lastInd) {
        int bufSize = lastInd - firstInd + 1; //Находим размер буфера
        int* buf = new int[bufSize];
        for (int i = 0; i < bufSize; ++i) //Заполняем буфер
            buf[i] = arr[firstInd + i];


        int mid = (lastInd - firstInd) / 2; //Находим разницу деленую на 2
        int j = 0, k = mid + 1; //Определяем индексы начала двух частей
        //Объединяем массивы
        for (int i = firstInd; i <= lastInd; ++i) {
            if (k < bufSize && buf[k] < buf[j])
                arr[i] = buf[k++];
            else if (j <= mid)
                arr[i] = buf[j++];
        }
        delete[] buf;
    }

    /**
     * Индекс левого ребенка
     * @param i индекс родителя
     * @return индекс левого ребенка
     */
    static int left(int i) {
        return 2 * i + 1;
    }

    /**
     * Индекс правого ребенка
     * @param i индекс родителя
     * @return индекс правого ребенка
     */
    static int right(int i) {
        return 2 * i + 2;
    }

    /**
     * Создание кучи
     * @param arr сортируемый массив
     * @param i индекс элемента
     * @param size размер кучи
     */
    static void heapify(int *arr, int i, int size) {
        int l = left(i);
        int r = right(i);
        int largest = l < size && arr[l] > arr[i] ? l : i;
        largest = r < size && arr[r] > arr[largest] ? r : largest;

        if (largest != i) {
            std::swap(arr[i], arr[largest]);
            if (largest * 2 + 1 < size)
                heapify(arr, largest, size);
        }
    }

    /**
     * Быстрая сортировка
     * @param arr сортируемый массив
     * @param in начальный индекс сортировки
     * @param ik конечный индекс сортировки
     */
    static void qSort(int* arr, int in, int ik) {
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

        if (in < j)
            qSort(arr, in, j);

        if (ik > i)
            qSort(arr, i, ik);
    }

    /**
     * Сортировка слиянием
     * @param arr сортируемый массив
     * @param firstInd начальный идекс сортировки
     * @param lastInd конечный индекс сортировки
     */
    static void mSort(int* arr, int firstInd, int lastInd) {
        if (firstInd >= lastInd) //Если разбить невозможно (Элемент один)
            return;

        int mid = firstInd + (lastInd - firstInd) / 2;
        //Сортируем 2 части массива отдельно
        mSort(arr, firstInd, mid); //Сортировка первой части
        mSort(arr, mid + 1, lastInd); //Сортировка второй части
        merge(arr, firstInd, lastInd); //Слияние двух частей в одну
    }

public:

    //Итерационные сортировки
    /**
     * Сортировка пузырьком
     * @param arr сортируемый массив
     * @param size размер массива
     */
    static void bubbleSort(int* arr, int size) {
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size - i - 1; ++j)
                if (arr[j] > arr[j + 1]) {
                    int t = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = t;
                }
    }

    /**
     * Сортировка выбором
     * @param arr сортируемый массив
     * @param size размер массива
     */
    static void selectSort(int* arr, int size) {
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
                arr[minIndex] = arr[i];
                arr[i] = min;
            }
        }
    }

    /**
     * Сортировка вставками
     * @param arr сортируемый массив
     * @param size размер массива
     */
    static void insertionSort(int* arr, int size) {
        for (int i = 1; i < size; ++i) {
            int val = arr[i];
            int j = i;
            while (j > 0 && arr[j - 1] > val) {
                arr[j] = arr[j - 1];
                j--;
            }
            if (j != i)
                arr[j] = val;
        }
    }

    //Линейные сортировки
    /**
     * Сортировка подсчетом
     * @param arr сортируемый массив
     * @param size размер массива
     */
    static void countingSort(int *arr, int size) {
        //Находим максимально возможное количество различных чисел
        //в массиве по его минимуму и максимуму
        int min = 1000000, max = 0;
        for(int i = 0; i < size; ++i) {
            if (arr[i] < min) min = arr[i];
            if (arr[i] > max) max = arr[i];
        }

        int counterSize = (max - min) + 1;
        int* counters = new int[counterSize];
        for (int i = 0; i < counterSize; ++i)
            counters[i] = 0; //Зануляем массив

        //Подсчитываем количество непревышающих элементов для
        //каждого числа исходного массива
        for (int i = 0; i < size; ++i)
            ++counters[arr[i] - min];
        for (int i = 1; i < counterSize; ++i)
            counters[i] += counters[i - 1];

        int* result = new int[size]; //Отсортированный массив
        for (int i = size - 1; i >= 0; --i)
            result[--counters[arr[i] - min]] = arr[i]; //Сортируем массив

        for (int i = 0; i < size; ++i)
            arr[i] = result[i]; //Копируем отсортированный массив в исходный

        //Освобождаем память
        delete[] counters;
        delete[] result;
    }

    /**
     * Цифровая сортировка
     * @param arr сортируемый массив
     * @param size размер массива
     */
    static void radixSort(int *arr, int size) {
        int maxDischargeNum = 0; //Максимальное кол-во разрядов числа в 8-ом представлении

        //Находим maxDischargeNum
        for (int i = 0; i < size; ++i)
        {
            int number = arr[i];
            int j = 0;
            while (number > 0) {
                number /= 10;
                j++;
            }

            if (j > maxDischargeNum)
                maxDischargeNum = j;
        }

        for (int i = 0; i < maxDischargeNum + 1; ++i)
            countingSortForDigits(arr, size, i);
    }

    //Рекурсивные сортировки
    /**
     * Обертка сортивки слиянием для более удобного обращения
     * @param arr сортируемый массив
     * @param size размер массива
     */
    static void mergeSort(int* arr, int size) {
        mSort(arr, 0, size - 1);
    }

    /**
     * Обертка быстрой сортировки для более удобного обращения
     * @param arr
     * @param size
     */
    static void quickSort(int* arr, int size) {
        qSort(arr, 0, size - 1);
    }

    /**
     * Сортировка кучей
     * @param arr сортируемый массив
     * @param size размер массива
     */
    static void heapSort(int *arr, int size)
    {
        for (int i = size / 2 - 1; i >= 0; --i)
            heapify(arr, i, size);

        for (int i = size - 1; i >= 0; --i) {
            std::swap(arr[0], arr[i]);
            heapify(arr, 0, i);
        }
    }
};
