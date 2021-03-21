#include <iostream>
#include <fstream>

class ReadWriter
{
private:

    std::fstream fin;
    std::fstream fout;

public:

    ~ReadWriter()
    {
        fin.close();
        fout.close();
    }

    ReadWriter()
    {
        fin.open("input.txt", std::ios::in);
        fout.open("output.txt", std::ios::out);
    }

    int readInt()
    {
        if (!fin.is_open())
            throw std::ios_base::failure("file not open");

        int n = 0;
        fin >> n;
        return n;
    }

    //This method only fill array. You should create it before call this method.
    void readArray(int* arr, int length)
    {
        if (!fin.is_open())
            throw std::ios_base::failure("file not open");

        for (int i = 0; i < length; i++)
            fin >> arr[i];
    }

    void writeArray(int* arr, int n)
    {
        if (!fout.is_open())
            throw std::ios_base::failure("file not open");

        for (int i = 0; i < n; i++)
            fout << arr[i] << " ";
    }
};


//int main() {
//    int n;
//    std::cin >> n;
//    int *arr = new int[n];
//    for (int i = 0; i < n; ++i) {
//        std::cin >> arr[i];
//    }
//
//    heapSort(arr, n);
//
//    for (int i = 0; i < n; ++i) {
//        std::cout << arr[i] << " ";
//    }
//
//    delete[] arr;
//    return 0;
//}