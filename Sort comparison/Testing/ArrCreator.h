#include "iostream"
#include "Sortings.h"

class ArrCreator {
private:
    int size = 4100; //Размер возвращаемого массива
    int *arrFrom0to5; //Массив с элементами от 0 до 5
    int *arrFrom0to4000; //Массив с элементами от 0 до 4000
    int *almostSortedArr; //Почти отсортированный массив
    int *reverseSortArr; //Обратная сортированный массив

    /**
     * Переворачивает массив
     * @param arr исходный массив
     */
    void reverseArr(int* arr) {
        int *brr = new int[4100];
        for (int i = 0, j = 4099; i < 4100; ++i, --j)
            brr[j] = arr[i];

        for (int i = 0; i < 4100; ++i)
            arr[i] = brr[i];

        delete[] brr;
    }

    /**
     * Перемешиваем несколько эелементов в массиве (4 пары на 1000)
     * @param arr исходный массив
     */
    void shuffle(int* arr) {
        for (int i = 0; i < 4100 / 250; ++i) {
            int index1 = std::rand() % 250;
            int index2 = std::rand() % 250;
            std::swap(arr[index1 + i * 250], arr[index2 + i * 250]);
        }
    }

    /**
     * Создаем массив
     * @param reverseSort обратноотсортированный массив
     * @param almostSorted почти отсортированный массив
     * @param from0to4000 массив с элементами от 0 до 4000
     * @return указатель на массив удовлетворяющий переданным
     * настройкам
     */
    int* createArr(bool reverseSort, bool almostSorted, bool from0to4000) {
        int *arr = new int[4100];

        if (reverseSort || almostSorted) {
            int j = 4100;
            for (int i = 0; i < 4100; ++i)
                arr[i] = j--;
            Sortings::quickSort(arr, 4100);

            if (reverseSort) reverseArr(arr);
            if (almostSorted) shuffle(arr);
        } else {
            int maxVal = 5;
            if (from0to4000) maxVal = 4000;
            for (int i = 0; i < 4100; ++i)
                arr[i] = std::rand() % (maxVal + 1);
        }

        return arr;
    }

    /**
     * Копирует массив
     * @param arr указатель на копируемый массив
     * @param size размер копируемого массива
     * @return указатель на скопированный массив
     */
    static void copyArr (const int* from, int* to, int size) {
        for (int i = 0; i < size; ++i)
            to[i] = from[i];
    }

public:

    /**
     * Создает генератор массивов
     */
    ArrCreator() {
        arrFrom0to5 = createArr(false, false, false);
        arrFrom0to4000 = createArr(false, false, true);
        almostSortedArr = createArr(false, true, false);
        reverseSortArr = createArr(true, false, false);
    }

    /**
     * Устанавливает размер возвращаемого массива
     * @param value
     */
    void setSize(int value) { size = value; }

    /**
     * Возвращает размер возвращаемого массива
     * @return
     */
    int getSize() { return size; }

    /**
     * Возвращает копию массива нужного типа
     * @param i типа массива
     * @return копия массива нужного типа
     */
    void getArr(int i, int* arr) {
        switch (i) {
            case 0:
                copyArr(arrFrom0to5, arr, size);
                return;
            case 1:
                copyArr(arrFrom0to4000, arr, size);
                return;
            case 2:
                copyArr(reverseSortArr, arr, size);
                return;
            case 3:
                copyArr(almostSortedArr, arr, size);
                return;
            default:
                return;
        }
    }

    /**
     * Чистит памят от массивов
     */
    ~ArrCreator() {
        delete[] arrFrom0to5;
        delete[] arrFrom0to4000;
        delete[] almostSortedArr;
        delete[] reverseSortArr;
    }
};

