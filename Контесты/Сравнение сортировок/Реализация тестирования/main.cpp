#include <vector>
#include <map>
#include "Tester.h"
#include "fstream"

/* АиСД-2, 2021, задание 5
 * Назмутдинов Роман Ренатович БПИ-194
 * Clion (компилятор mingw)
 *
 * Релизовано:
 * + 8 сортировок (представленны ниже)
 * + Генерация массивов
 * + Тестирование сортировок
 * + Замер времени
 * + Создание csv таблиц
 * + input и output для своих тестов
 * + Файл с неверными результатами сортировки (если такое встретилось)
 * + Требуемые графики
 * + Пояснение к графикам и вывод
 *
 * Не реализовано:
 *
 * Вроде все есть
 */

//Все выбраные мной сортировки
std::map<std::string, void (*)(int*, int)> sortings {
        {"Bubble Sort", Sortings::bubbleSort},
        {"Insertion Sort", Sortings::insertionSort},
        {"Select Sort", Sortings::selectSort},
        {"Counting Sort", Sortings::countingSort},
        {"Radix Sort", Sortings::radixSort},
        {"Merge Sort", Sortings::mergeSort},
        {"Quick Sort", Sortings::quickSort},
        {"Heap Sort", Sortings::heapSort}
};

//Названия видов массивов
std::vector<std::string> methodsName = {
        "Random nums 0-5",
        "Random nums 0-4000",
        "Reverse arr",
        "Almost sort arr"
};

//Файлы
std::string output = "output.txt"; //Для вывода результатов
std::string input = "input.txt"; //Для передачи своего тестового массива
std::string wa = "wa.txt"; //Для неверных ответов
std::string path1 = "table1.csv";
std::string path2 = "table2.csv";
std::string path3 = "resultForTestFromInputFile.csv";

/* Создает автоматические тесты и прогоняет
 * сортировки по этим тестам*/
void autoTest() {
    std::fstream fs1, fs2;
    fs1.open(path1, std::fstream::out);
    fs2.open(path2, std::fstream::out);

    if (fs1.is_open() && fs2.is_open()) {
        //Вывводим первую строку в таблице
        fs1 << "Method \\ Size;";
        fs2 << "Method \\ Size;";
        for (int i = 50; i <= 300; i += 50)
            fs1 << i << ";";
        for (int i = 100; i <= 4100; i += 1000)
            fs2 << i << ";";
        fs1 << "\n";
        fs2 << "\n";

        Tester tester(output, wa); //тестировщик
        ArrCreator creator; //генератор массивов
        for (auto &sorting : sortings) { // проходимся по сортировкам
            std::cout << "Testing " << sorting.first << "...\n";
            //Проверяем сортировку на 4-х разных видах массива
            for (int i = 0; i < 4; ++i) {
                std::string sortingName = sorting.first;
                fs1 << sortingName << " " << methodsName[i] << ";";
                fs2 << sortingName << " " << methodsName[i] << ";";

                //Тестируем сортировку на массиве размером от 50 до 300
                for (int j = 50; j <= 300; j += 50) {
                    creator.setSize(j);
                    unsigned long long tact = tester.autoTest(creator, i, sortingName, sorting.second);
                    fs1 << tact << ";";
                }
                fs1 << "\n";

                //Тестируем сортировку на массиве размером от 100 до 4100
                for (int j = 100; j <= 4100; j += 1000) {
                    creator.setSize(j);
                    unsigned long long tact = tester.autoTest(creator, i, sortingName, sorting.second);
                    fs2 << tact << ";";
                }
                fs2 << "\n";
            }
        }
    } else
        std::cout << "can\'t open file " << path1 << " or " << path2 << "\n";
}

// Тестирует сортировки на массиве из файла
void testFromFile() {
    Tester tester(input, output, wa);
    std::fstream fs;
    fs.open(path3, std::fstream::out);
    if (fs.is_open()) {
        if (tester.isInputOpen()) {
            for (auto &sorting : sortings) { //Прохоидмся по сортировкам
                std::string sortingName = sorting.first;
                std::cout << "Testing " << sortingName << "...\n";
                fs << sortingName << ";";

                //Тестируем сортировку
                unsigned long long tact = tester.test(sortingName, sorting.second);
                fs << tact << ";";
                fs << "\n";
            }
        }
    } else
        std::cout << "can\'t open file " << path3 << "\n";
}

int main() {
    int num = 0;
    //Меню
    do {
        std::cout << "1. Auto test\n";
        std::cout << "2. Test from file\n\n";
        std::cout << "Enter number:";
        std::cin >> num;
        if (num == 1)
            autoTest();
        else if (num == 2)
            testFromFile();
    } while (num > 2 || num < 1);

    return 0;
}
