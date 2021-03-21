#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void arrBrs(std::string& s, std::vector<int>& br)
{
    br = std::vector<int>(s.length());
    for (int i = 0; i < s.size() - 1; i++) {
        if (s[i + 1] == s[br[i]])
            br[i + 1] = br[i] + 1;
        else {
            int j = i;
            while (br[j] != 0 && s[i + 1] != s[br[br[j] - 1]])
                j = br[j] - 1;

            if (br[j] == 0)
                br[i + 1] = s[i + 1] == s[br[j]] ? 1 : 0;
            else
                br[i + 1] = br[br[j] - 1] + 1;
        }
    }
}

//Основная задача - реализовать данный метод
//Можно изменить передачу параметров на ссылки (&)
//Можно добавлять любое количество любых вспомогательных методов, структур и классов
void getSubstrings(std::string& str, std::string& substr, std::vector<int>& res)
{
    std::vector<int> brs;
    arrBrs(substr, brs);
    int strInd = 0;
    int substrInd = 0;

    while(strInd < str.length()) {
        if (str[strInd] == substr[substrInd]) {
            strInd++;
            substrInd++;
            if (substrInd == substr.length())
                res.push_back(strInd - substrInd);
        } else {
            if (substrInd == 0) {
                ++strInd;
                if (strInd == str.length())
                    break;
            } else
                substrInd = brs[substrInd - 1];
        }
    }
}

//Не изменять метод main без крайней необходимости
//ОБЯЗАТЕЛЬНО добавить в комментариях подробные пояснения и причины побудившие вас изменить код этого метода.
int main()
{
    std::string t;
    std::string p;
    std::vector<int> res;

    std::ifstream fin;
    fin.open("input.txt");
    if (fin.is_open())
    {
        getline(fin, t);
        getline(fin, p);
        fin.close();
    }

    getSubstrings(t, p, res);

    std::fstream fout;
    fout.open("output.txt", std::ios::out);
    fout << res.size() << "\n";
    for (std::vector<int>::const_iterator i = res.begin(); i != res.end(); ++i)
        fout << *i << "\n";
    fout.close();

    return 0;
}


int* getBorders(std::string &s)
{
    int len = s.length();
    int *br = new int[len];
    br[0] = 0;

    int curBorderLen = 0;
    for (int i = 1; i < len; ++i) {
        while (curBorderLen > 0 && s[curBorderLen] != s[i])
            curBorderLen = br[curBorderLen - 1];

        if (s[curBorderLen] == s[i])
            ++curBorderLen;

        br[i] = curBorderLen;
    }
    return br;
}

int* getBrs(std::string& str) {
    int *br = getBorders(str);
    int *brs = new int[str.length()];
    brs[0] = 0;
    str += "]";

    for (int i = 1; i < str.length() - 1; ++i) {
        if (str[br[i]] != str[i + 1])
            brs[i] = br[i];
        else
            brs[i] = brs[br[i]];
    }
    str.erase(str.length() - 1);
    delete[] br;
    return brs;
}