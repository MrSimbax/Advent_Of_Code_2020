#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <array>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using Int = long long int;

constexpr Int PREAMBLE_LENGTH = 25;

std::vector<Int> loadNumbers(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::vector<Int> numbers;
    numbers.push_back(0);
    Int x;
    while (ifs >> x)
        numbers.push_back(x);
    std::sort(numbers.begin(), numbers.end());
    numbers.push_back(*(numbers.end() - 1) + 3);
    return numbers;
}

std::vector<Int> calculateDifferences(const std::vector<Int>& numbers)
{
    std::vector<Int> differences(numbers.size(), 0);
    for (Int i = 1; i < numbers.size(); ++i)
        differences[i] = numbers[i] - numbers[i-1];
    return differences;
}

Int part1(const std::vector<Int>& numbers)
{
    auto differences = calculateDifferences(numbers);
    Int onesCount = std::count(differences.begin(), differences.end(), 1);
    Int threesCount = std::count(differences.begin(), differences.end(), 3);
    return onesCount * threesCount;
}

Int part2(const std::vector<Int>& numbers)
{
    // counts[i] = number of arrangements (paths) to adapter at index i
    std::vector<Int> counts(numbers.size());
    counts[0] = 1;
    for (Int i = 1; i < numbers.size(); ++i)
    {
        Int sum = 0;
        for (Int j = 1; j < 4 && i - j >= 0; ++j)
            if (numbers[i] - numbers[i - j] <= 3)
                sum += counts[i - j];
        counts[i] = sum;
    }
    return counts[numbers.size() - 1];
}

int main()
{
    const auto& numbers = loadNumbers("day10.txt");
    std::cout << "part 1: " << part1(numbers) << std::endl;
    std::cout << "part 2: " << part2(numbers) << std::endl;
}
