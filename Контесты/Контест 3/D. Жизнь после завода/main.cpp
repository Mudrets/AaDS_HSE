#include <iostream>

int left(int i) {
    return 2 * i + 1;
}

int right(int i) {
    return 2 * i + 2;
}

void heapfy(int *arr, int i, int size) {
    int l = left(i);
    int r = right(i);
    int largest = l < size && arr[l] > arr[i] ? l : i;
    largest = r < size && arr[r] > arr[largest] ? r : largest;

    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapfy(arr, largest, size);
    }
}

void heapSort(int *numbers, int array_size)
{
    for (int i = array_size - 1; i >= 0; --i)
        heapfy(numbers, i, array_size);

    for (int i = array_size - 1; i >= 0; --i) {
        std::swap(numbers[0], numbers[i]);
        heapfy(numbers, 0, i);
    }
}

//Находит первый превышающий elem элемент
int binarySearch(int* arr, int size, int elem) {
    int left = 0;
    int right = size - 1;
    while (left < right) {
        int middle = (left + right) / 2;
        if (elem >= arr[middle])
            left = middle + 1;
        else
            right = middle - 1;
    }
    while (arr[left] <= elem && left < size)
        left++;
    return left;
}

int countIntersections(int* arrX, int* arrY, int size, int point) {
    int firstLargerX = binarySearch(arrX, size, point);
    int firstLargerY = binarySearch(arrY, size, point);

    if (firstLargerY == size && arrY[firstLargerY - 1] == point)
        firstLargerY--;
    while (arrY[firstLargerY] >= point && firstLargerY >= 0 && firstLargerY < size)
        firstLargerY--;
    if (firstLargerY + 1 < size && arrY[firstLargerY + 1] >= point)
        firstLargerY++;

    int countLargerX = size - firstLargerX;
    int countLessY = firstLargerY;

    return size - countLargerX - countLessY;
}

int main() {
    std::cin.tie(NULL);
    std::cout.tie(NULL);

    int n;
    std::cin >> n;

    int *arrX = new int[n];
    int *arrY = new int[n];

    for (int i = 0; i < n; ++i)
        std::cin >> arrX[i] >> arrY[i];

    heapSort(arrX, n);
    heapSort(arrY, n);

    int k;
    std::cin >> k;
    for (int i = 0; i < k; ++i) {
        int time;
        std::cin >> time;
        std::cout << countIntersections(arrX, arrY, n, time) << "\n";
    }

    return 0;
}

//int main() {
//    int n;
//    std::cin >> n;
//
//    Student *students = new Student[n];
//    Ski *skis = new Ski[n];
//
//    std::vector<int> vec = {1, 2, 6, 9, 13};
//    int a = binarySearch(vec, 13);
//    char ch;
//    for (int i = 0; i < n; i++)
//    {
//        std::cin >> students[i].id;
//        std::cin >> students[i].height;
//        std::cin >> ch; // ;
//    }
//
//    for (int i = 0; i < n; i++)
//    {
//        std::cin >> skis[i].id;
//        std::cin >> skis[i].length;
//        std::cin >> ch; // ;
//    }
//
//    findPairs(students, skis, n);
//
//    for (int i = 0; i < n; ++i) {
//        std::cout << students[i].id << " ";
//        std::cout << skis[i].id << "; ";
//    }
//
//    delete[] skis;
//    delete[] students;
//    return 0;
//}
