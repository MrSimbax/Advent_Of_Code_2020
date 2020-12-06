#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>

using Int = long long int;

std::vector<Int> loadSortedEntries(const std::string& inputFilename)
{
    std::ifstream ifs{inputFilename};
    Int entry{0};
    std::vector<Int> entries;
    while (ifs >> entry)
    {
        entries.push_back(entry);
    }
    std::sort(entries.begin(), entries.end());
    return entries;
}

Int part1(const std::vector<Int>& entries, Int n)
{
    for (auto x : entries)
    {
        Int y = n - x;
        if (std::binary_search(entries.begin(), entries.end(), y))
        {
            return x * y;
        }
    }
    return -1;
}

Int part2(const std::vector<Int>& entries)
{
    for (auto x : entries)
    {
        Int m = part1(entries, 2020 - x);
        if (m >= 0)
        {
            return m * x;
        }
    }
    return -1;
}

int main()
{
    std::cout << part1(loadSortedEntries("day1.txt"), 2020) << std::endl;
    std::cout << part2(loadSortedEntries("day1.txt")) << std::endl;
}
