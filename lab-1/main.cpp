#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdint>

struct Data {
    std::string rareDate_;
    uint64_t value_;
    uint32_t correctDate_;
};

bool splitAndValidate(const std::string& date, uint32_t& correctDate) {
    std::array<std::string, 3> parts;
    size_t idx = 0;
    std::stringstream ss(date);
    std::string part;

    while (std::getline(ss, part, '.')) {
        parts[idx++] = part;
    }

    if (parts.size() != 3) return false;

    for (const auto& p : parts) {
        if (p.empty() || !std::all_of(p.begin(), p.end(), ::isdigit)) {
            return false;
        }
    }

    const uint16_t day = std::stoi(parts[0]);
    const uint16_t month = std::stoi(parts[1]);
    const uint16_t year = std::stoi(parts[2]);

    if (day < 1 || day > 31) return false;
    if (month < 1 || month > 12) return false;
    if (year < 1) return false;

    correctDate = year * 10000 + month * 100 + day;
    
    return true;
}

void countingSort(std::vector<Data>& arrayOfPairs, std::vector<Data>& buffer, int divisor) {
    std::array<size_t, 10> count{0};    
    const size_t size = arrayOfPairs.size();

    for (size_t i = 0; i < size; ++i) {
        count[(arrayOfPairs[i].correctDate_ / divisor) % 10]++;
    }

    for (size_t i = 1; i < 10; ++i) {
        count[i] += count[i - 1];
    }

    for (int i = size - 1; i >= 0; --i) {
        buffer[--count[(arrayOfPairs[i].correctDate_ / divisor) % 10]] = std::move(arrayOfPairs[i]);
    }

    arrayOfPairs.swap(buffer);
}

void radixSort(std::vector<Data>& arrayOfPairs) {
    if (arrayOfPairs.empty()) return;

    std::vector<Data> buffer(arrayOfPairs.size());

    for (int divisor = 1; divisor <= 10000000; divisor *= 10) {
        countingSort(arrayOfPairs, buffer, divisor);
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::vector<Data> inputPair;
    inputPair.reserve(617800);
    std::string line;
    line.reserve(128);

    while (std::getline(std::cin, line)) {
        if (line.empty() || line.find('\t') == std::string::npos) continue;

        Data elementOfData;
        elementOfData.rareDate_ = line.substr(0, line.find('\t'));
        elementOfData.value_ = std::stoull(line.substr(line.find('\t') + 1));

        uint32_t correctDate;
        if (!splitAndValidate(elementOfData.rareDate_, correctDate)) continue;

        elementOfData.correctDate_ = correctDate;
        inputPair.emplace_back(std::move(elementOfData));
    }

    radixSort(inputPair);

    for (size_t i = 0; i < inputPair.size(); ++i) {
        std::cout << inputPair[i].rareDate_ << '\t' << inputPair[i].value_ << '\n';
    }

    return 0;
}
