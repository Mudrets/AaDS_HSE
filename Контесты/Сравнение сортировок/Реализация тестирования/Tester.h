#include "ArrCreator.h"
#include "fstream"

class Tester {
private:
    std::fstream fin; //Для считывания массива из файла
    std::fstream fout; //Для вывода отсортированных массивов
    std::fstream fwa; //Для вывода ошибочных сортировок
    int *testArr; //Тестовый массив из файла
    int sizeTestArr = -1; //Размер тестового массива
    typedef unsigned long long ull; //Для подсчета тактов

    /**
     * Считает количество тактов
     */
    static inline ull rdtsc() {
        unsigned int lo, hi;
        asm volatile ("rdtsc\n" : "=a" (lo), "=d" (hi));
        return ((ull)hi << 32) | lo;
    }

    /**
     * Проверяет правильность сортировки массива
     * @param arr проверяемый массив
     * @param size размер проверяемого массива
     * @return True - массив отсортирован верно;
     * False - неверно
     */
    static bool checkSort(int* arr, int size) {
        for (int i = 1; i < size; ++i)
            if (arr[i] < arr[i - 1])
                return false;
        return true;
    }

    /**
     * Выводит массив, отсортированный массив и название сортировки
     * @param fs поток для вывода информации в файл
     * @param sortName название сортировки
     * @param arr исходный массив
     * @param sortArr отсортированный массив
     * @param size размер массивов
     */
    static void writeArr(std::fstream &fs, std::string &sortName, int* arr, int* sortArr, int size) {
        if (fs.is_open()) {
            fs << sortName << ":\n";
            fs << "input arr: ";
            for (int i = 0; i < size; ++i)
                fs << arr[i] << " ";
            fs << "\noutput arr: ";
            for (int i = 0; i < size; ++i)
                fs << sortArr[i] << " ";
            fs << "\n\n";
        }
    }

    /**
     * Копирует тестовый массив в массив с переданным указателем
     * @param arr указатель на массив в который нужно скопировать тестовый массив
     */
    void copyTestArr(int* arr) {
        for (int i = 0; i < sizeTestArr; ++i)
            arr[i] = testArr[i];
    }

    /**
     * Считывает массив из файла
     */
    void readArr() {
        if (!fin.is_open())
            return;

        fin >> sizeTestArr;
        testArr = new int[sizeTestArr];
        for (int i = 0; i < sizeTestArr; ++i)
            fin >> testArr[i];
    }

public:

    /**
     * Создает тестировщик
     * @param inputFile путь файла с тестовым массивом
     * @param outputFile путь к файлу для выходных данных
     * @param waFile файл с ошибочными ответами
     */
    Tester(std::string &inputFile, std::string &outputFile, std::string &waFile) {
        fin.open(inputFile, std::ios::in);
        if (!fin.is_open())
            std::cout << "Tester:\tcan\'t open input file";
        else
            readArr();

        fout.open(outputFile, std::ios::out);
        if (!fout.is_open()) std::cout << "Tester:\tcan\'t open output file";

        fwa.open(waFile, std::ios::out);
        if (!fwa.is_open()) std::cout << "Tester:\tcan\'t open wa file";
    }

    /**
     * Создает тестировщик
     * @param outputFile файл для выходных данных
     * @param waFile файл с ошибочными ответами
     */
    Tester(std::string &outputFile, std::string &waFile) {
        fout.open(outputFile, std::ios::out);
        if (!fout.is_open()) std::cout << "Tester:\tcan\'t open output file";

        fwa.open(waFile, std::ios::out);
        if (!fwa.is_open()) std::cout << "Tester:\tcan\'t open wa file";
    }

    /**
     * Проверяет, что файл для чтения массива открыт
     * @return
     */
    bool isInputOpen() {
        return fin.is_open();
    }

    /**
     * Автоматическое тестирование сортировок
     * @param creator генератор массива
     * @param kindOfArr тип генерируемого массива
     * @param sortingName название сортировки
     * @param f функция сортировки
     * @return среднее количество тактов, нужных для выполнения сортировки
     */
    unsigned long long autoTest(ArrCreator &creator, int kindOfArr, std::string &sortingName, void (*f)(int*, int)) {
        int size = creator.getSize(); //размер массива
        unsigned long long sum = 0; //суммарное количество тактов

        for (int i = 0; i < 33; ++i) {
            int *arr = new int[size];
            creator.getArr(kindOfArr, arr);

            if (i < 3) //Первые 3 сортировки не учитываем в итоге
                (*f)(arr, size);
            else {
                //Замеряем время выполнения
                ull start = rdtsc();
                (*f)(arr, size);
                sum += (rdtsc() - start);

                //Выводим результаты сортировки в файл
                if (i == 32) {
                    int *inputArr = new int[size];
                    creator.getArr(kindOfArr, inputArr);
                    if (!checkSort(arr, size)) //проверяем правильность сортировки
                        writeArr(fwa, sortingName, inputArr, arr, size);
                    writeArr(fout, sortingName, inputArr, arr, size);
                    delete[] inputArr;
                }
            }
            delete[] arr;
        }
        return sum / 30; //Определяем среднее время сортировки
    }

    /**
     * Тестирование на основе тестового массива
     * @param sortingName название сортировки
     * @param f функция сортировки
     * @return среднее количество тактов, нужных для выполнения сортировки
     */
    unsigned long long test(std::string &sortingName, void (*f)(int*, int)) {
        unsigned long long sum = 0; //суммарное количество тактов

        for (int i = 0; i < 13; ++i) {
            int *arr = new int[sizeTestArr];
            copyTestArr(arr); //Копируем тестовый массив

            if (i < 3) //Первые 3 сортировки не учитываем в итоге
                (*f)(arr, sizeTestArr);
            else {
                //Замеряем время
                ull start = rdtsc();
                (*f)(arr, sizeTestArr);
                sum += (rdtsc() - start);

                //Выводим результаты сортировки в файл
                if (i == 12) {
                    int *inputArr = new int[sizeTestArr];
                    copyTestArr(inputArr);
                    if (!checkSort(arr, sizeTestArr)) //проверяем правильность сортировки
                        writeArr(fwa, sortingName, inputArr, arr, sizeTestArr);
                    writeArr(fout, sortingName, inputArr, arr, sizeTestArr);
                    delete[] inputArr;
                }
            }
            delete[] arr;
        }
        return sum / 10; //Определяем среднее время сортировки
    }

    ~Tester() {
        //Освобождаем память и закрываем потоки
        if (sizeTestArr != -1)
            delete[] testArr;
        fin.close();
        fout.close();
        fwa.close();
    }

};