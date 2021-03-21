#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

//Основная задача - реализовать данный метод
//Результат поместить в переменную res, она как раз доступна для изменения
//Можно добавлять любое количество любых вспомогательных методов, структур и классов
void getBorders(std::string &s, std::string& res)
{
    int n = s.size();
    for (int i = 1; i < n; ++i) {
        std::string prefix = s.substr(0, i);
        std::string suffix = s.substr(n - i, i);
        if (prefix == suffix)
            res += prefix + "\n";
    }
}

//Не изменять метод main без крайней необходимости
//ОБЯЗАТЕЛЬНО добавить в комментариях подробные пояснения и причины побудившие вас изменить код этого метода.
int main()
{
    std::string input;
    std::string res;

    std::ifstream fin;
    fin.open("input.txt");
    if (fin.is_open())
    {
        getline(fin, input);
        fin.close();
    }

    getBorders(input, res);

    std::fstream fout;
    fout.open("output.txt", std::ios::out);
    fout << res;
    fout.close();

    return 0;
}
