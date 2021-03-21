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
    void readArrays(int* arrId, int* arrWeight, int length)
    {
        if (!fin.is_open())
            throw std::ios_base::failure("file not open");

        for (int i = 0; i < length; i++)
            fin >> arrId[i] >> arrWeight[i];
    }

    void writeArrays(int* arrId,int* arrWeight, int n)
    {
        if (!fout.is_open())
            throw std::ios_base::failure("file not open");

        for (int i = 0; i < n; i++)
            fout << arrId[i] << "\t" << arrWeight[i] << "\n";
    }

};
