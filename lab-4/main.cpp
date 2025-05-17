#include <iostream>
#include <string>
#include <vector>

std::vector<int> zFunction(const std::string& str) {
    int len = str.length();
    std::vector<int> zArr(len);
    int left = 0, right = 0;

    for (int idx = 1; idx < len; ++idx) {
        if (idx <= right)
            zArr[idx] = std::min(right - idx + 1, zArr[idx - left]);
    
        while (idx + zArr[idx] < len && str[zArr[idx]] == str[idx + zArr[idx]])
            ++zArr[idx];
    
        if (idx + zArr[idx] - 1 > right) {
            left = idx;
            right = idx + zArr[idx] - 1;
        }
    }
    return zArr;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text, pattern;
    std::cin >> text >> pattern;

    std::string concat = pattern + '$' + text;
    std::vector<int> zVals = zFunction(concat);
    int patLen = pattern.size();

    for (int i = patLen + 1; i < (int)zVals.size(); ++i) {
        if (zVals[i] == patLen) {
            std::cout << (i - patLen - 1) << "\n";
        }
    }
    return 0;
}
