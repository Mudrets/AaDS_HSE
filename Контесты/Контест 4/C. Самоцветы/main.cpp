#include <iostream>
#include <fstream>
#include <vector>

void quickSort(std::vector<int> &arr, int in, int ik) {
    int i = in;
    int j = ik;
    int pivot = arr[(i + j) / 2];

    while (i <= j) {
        while (i <= j && arr[i] < pivot)
            i++;

        while (j >= i && arr[j] > pivot)
            j--;

        if (i <= j && arr[i] >= pivot && arr[j] <= pivot)
            std::swap(arr[i++], arr[j--]);
    }

    if (in < j)
        quickSort(arr, in, j);

    if (ik > i)
        quickSort(arr, i, ik);
}

//Находит первый превышающий элемент
int binarySearch(std::vector<int> &vec, int size, int elem) {
    int left = 0;
    int right = size - 1;
    while (left < right) {
        int middle = (left + right) / 2;
        if (elem >= vec[middle])
            left = middle + 1;
        else
            right = middle - 1;
    }
    while (vec[left] <= elem && left < size)
        left++;
    return left;
}

unsigned long long formula(int ri, int gj, int bk) {
    long long a1 = (ri - gj);
    long long a2 = (gj - bk);
    long long a3 = (ri - bk);
    unsigned long long res = a1 * a1 + a2 * a2 + a3 * a3;
    return res;
}

std::vector<int> findOptimalTrio(std::vector<int> &currVec, std::vector<int> &firstArr,
                                 std::vector<int> &secondArr) {
    std::vector<int> ans(3);
    unsigned long long minDiff = -1;

    for (int i = 0; i < currVec.size(); ++i) {
        std::vector<int> fromFirst(2, 0);
        std::vector<int> fromSecond(2, 0);

        fromFirst[0] = binarySearch(firstArr, firstArr.size(), currVec[i]);
        if (firstArr.size() == 1) {
            fromFirst[0] = 0;
            fromFirst[1] = 0;
        } else if (fromFirst[0] == 0)
            fromFirst[1] = 1;
        else if (fromFirst[0] == firstArr.size()) {
            fromFirst[0] = firstArr.size() - 1;
            fromFirst[1] = fromFirst[0] - 1;
        } else
            fromFirst[1] = fromFirst[0] - 1;

        fromSecond[0] = binarySearch(secondArr, secondArr.size(), currVec[i]);
        if (secondArr.size() == 1) {
            fromSecond[0] = 0;
            fromSecond[1] = 0;
        } else if (fromSecond[0] == 0)
            fromSecond[1] = 1;
        else if (fromSecond[0] == secondArr.size()) {
            fromSecond[0] = secondArr.size() - 1;
            fromSecond[1] = fromSecond[0] - 1;
        } else
            fromSecond[1] = fromSecond[0] - 1;


        fromFirst[0] = firstArr[fromFirst[0]];
        fromFirst[1] = firstArr[fromFirst[1]];
        fromSecond[0] = secondArr[fromSecond[0]];
        fromSecond[1] = secondArr[fromSecond[1]];
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                if (formula(currVec[i], fromFirst[j], fromSecond[k]) < minDiff) {
                    minDiff = formula(currVec[i], fromFirst[j], fromSecond[k]);
                    ans[0] = currVec[i];
                    ans[1] = fromFirst[j];
                    ans[2] = fromSecond[k];
                }
            }
        }
    }

    return ans;
}

// Необходимо вернуть вектор ans, который содержит три элемента:
// ans[0] - вес найденного самоцвета красного цвета
// ans[1] - вес найденного самоцвета зеленого цвета
// ans[2] - вес найденного самоцвета синего цвета
std::vector<int> solve(std::vector<int>& r, std::vector<int>& g, std::vector<int>& b)
{
    std::vector<int> ans(3);

    quickSort(r, 0, r.size() - 1);
    quickSort(g, 0, g.size() - 1);
    quickSort(b, 0, b.size() - 1);

    std::vector<int> firstTrio = findOptimalTrio(r, g, b);
    unsigned long long diff = formula(firstTrio[0], firstTrio[1], firstTrio[2]);

    std::vector<int> secondTrio = findOptimalTrio(g, r, b);
    if (formula(secondTrio[0], secondTrio[1], secondTrio[2]) < diff) {
        diff = formula(secondTrio[0], secondTrio[1], secondTrio[2]);
        ans[0] = secondTrio[1];
        ans[1] = secondTrio[0];
        ans[2] = secondTrio[2];
    } else {
        ans[0] = firstTrio[0];
        ans[1] = firstTrio[1];
        ans[2] = firstTrio[2];
    }

    std::vector<int> thirdTrio = findOptimalTrio(b, r, g);
    if (formula(thirdTrio[0], thirdTrio[1], thirdTrio[2]) < diff) {
        ans[0] = thirdTrio[1];
        ans[1] = thirdTrio[2];
        ans[2] = thirdTrio[0];
    }

    return ans;
}

int main() {
    std::fstream fin;
    std::fstream fout;
    fin.open("input.txt",std::ios::in);
    fout.open("output.txt",std::ios::out);

    if(fin.is_open()) {
        int t;
        fin >> t;
        while (t--) {
            int nr, ng, nb;
            fin >> nr >> ng >> nb;

            std::vector<int> r(nr), g(ng), b(nb);
            for (int i = 0; i < nr; ++i)
                fin >> r[i];
            for (int i = 0; i < ng; ++i)
                fin >> g[i];
            for (int i = 0; i < nb; ++i)
                fin >> b[i];

            std::vector<int> ans = solve(r, g, b);

            fout << ans[0] << ' ' << ans[1] << ' ' << ans[2] << "\n";
        }
    }
    fin.close();
    fout.close();
    return 0;
}