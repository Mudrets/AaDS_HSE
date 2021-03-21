#include <iostream>

int main() {
    int rise = 0; //Рост гнома
    int max = 99; //Максимальное значение роста
    int secondMax = 99; //Второй максимум роста

    do {
        if (rise >= 100 && rise <= 150)
            if (rise >= max) {
                secondMax = max;
                max = rise;
            } else if (rise >= secondMax)
                secondMax = rise;
            std::cin >> rise;
    } while (rise != 0);

    std::cout << max << std::endl << secondMax;
    return 0;
}
