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
    Int x;
    while (ifs >> x)
        numbers.push_back(x);
    return numbers;
}

template<typename It>
bool isSumOfTwoNumbers(It begin, const It end, Int theSum)
{
    for (auto it = begin; it != end; ++it)
    {
        Int x = *it;
        Int y = theSum - x;
        if (std::find(begin, end, y) != end)
            return true;
    }
    return false;
}

bool isNumberValid(Int i, const std::vector<Int>& numbers)
{
    auto it = numbers.begin() + i;
    return isSumOfTwoNumbers(it - PREAMBLE_LENGTH, it, numbers[i]);
}

Int part1(const std::vector<Int>& numbers)
{
    for (Int i = PREAMBLE_LENGTH; i < numbers.size(); ++i)
        if (!isNumberValid(i, numbers))
            return numbers[i];
    return -1;
}

Int part2(const std::vector<Int>& numbers, Int x)
{
    Int i = 0;
    Int j = 1;
    Int sum = numbers[i] + numbers[j];
    while (sum != x)
        if (sum > x)
            sum -= numbers[i++];
        else if (sum < x)
            sum += numbers[++j];
    auto [min, max] = std::minmax_element(numbers.begin() + i, numbers.begin() + j + 1);
    return *min + *max;
}

int main()
{
    const auto& numbers = loadNumbers("day9.txt");
    const auto& answer1 = part1(numbers);
    std::cout << "part 1: " << answer1 << std::endl;
    std::cout << "part 2: " << part2(loadNumbers("day9.txt"), answer1) << std::endl;
}
