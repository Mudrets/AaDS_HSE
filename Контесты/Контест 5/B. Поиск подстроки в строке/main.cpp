#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void getBorders(std::string &s, std::vector<int>& br)
{
    int len = s.length();
    br[0] = 0;

    int curBorderLen = 0;
    for (int i = 1; i < len; ++i) {
        while (curBorderLen > 0 && s[curBorderLen] != s[i])
            curBorderLen = br[curBorderLen - 1];

        if (s[curBorderLen] == s[i])
            ++curBorderLen;

        br[i] = curBorderLen;
    }
}

//Основная задача - реализовать данный метод
//Можно изменить передачу параметров на ссылки (&)
//Можно добавлять любое количество любых вспомогательных методов, структур и классов
void getSubstrings(string& source, string& substring, vector<int>& res)
{
    std::string newStr = substring + "*" + source;
    std::vector<int> br(newStr.length());
    getBorders(newStr, br);

    for (std::size_t i = substring.length() - 1; i < source.length(); ++i) {
        std::size_t brIndex = i + substring.length() + 1;
        if (br[brIndex] == substring.length())
            res.push_back(i - substring.length() + 1);
    }
}

//Не изменять метод main без крайней необходимости
//ОБЯЗАТЕЛЬНО добавить в комментариях подробные пояснения и причины побудившие вас изменить код этого метода.
int main()
{
    string t;
    string p;
    vector<int> res;

    ifstream fin;
    fin.open("input.txt");
    if (fin.is_open())
    {
        getline(fin, t);
        getline(fin, p);
        fin.close();
    }

    getSubstrings(t, p, res);

    fstream fout;
    fout.open("output.txt", ios::out);
    fout << res.size() << "\n";
    for (std::vector<int>::const_iterator i = res.begin(); i != res.end(); ++i)
        fout << *i << "\n";
    fout.close();

    return 0;
}
