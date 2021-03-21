#include <iostream>
#include <fstream>
#include <string>
#include <vector>


void findPalindrome(std::string& s, std::vector<int>& res) {
    int rightmostCenter = -1; //Центр самого парвого палиндрома
    int rightmostRadius = 0; //Радиус самого парвого палиндрома
    int mirroredCenter; //Центр зеркального палиндрома
    int mirroredRadius; //Радиус зеркального палиндрома

    for (int i = 0; i < s.size(); ++i) {
        int radius = 0; //Радус палиндрома с центром в текущем элементе
        if (i < rightmostCenter + rightmostRadius) {
            mirroredCenter = rightmostCenter - (i - rightmostCenter);
            mirroredRadius = (res[mirroredCenter] - 1) / 2;
            radius = std::min(mirroredRadius, rightmostCenter + rightmostRadius - i);
        }

        while (i - (radius + 1) >= 0 and i + (radius + 1) < s.size() &&
               s[i - (radius + 1)] == s[i + (radius + 1)])
            ++radius;

        if (i + radius > rightmostCenter + rightmostRadius) {
            rightmostCenter = i;
            rightmostRadius = radius;
        }

        res.push_back(radius * 2 + 1);
    }
}

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
        fin.close();
    }

    findPalindrome(t, res);

    std::fstream fout;
    fout.open("output.txt", std::ios::out);
    for (std::vector<int>::const_iterator i = res.begin(); i != res.end(); ++i)
        fout << *i << " ";
    fout.close();

    return 0;
}