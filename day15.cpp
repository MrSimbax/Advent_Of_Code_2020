#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <array>
#include <numeric>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <boost/algorithm/string.hpp>

using Int = long long int;

std::vector<Int> loadInput(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::string line;
    std::getline(ifs, line);
    std::vector<std::string> splitLine;
    boost::split(splitLine, line, boost::is_any_of(","));
    std::vector<Int> numbers;
    std::transform(splitLine.begin(), splitLine.end(), std::back_inserter(numbers),
                   [](const auto& n) { return std::stoll(n); });
    return numbers;
}

Int solve(const std::vector<Int>& numbers, Int limit)
{
    // number spoken -> last turn at which it was spoken
    std::vector<Int> game(limit + 1, -1);
    for (Int turn = 0; turn < numbers.size(); ++turn)
        game[numbers[turn]] = turn;
    Int lastNumberSpoken = numbers[numbers.size() - 1];
    for (Int turn = numbers.size(); turn < limit; ++turn)
    {
        auto lastSpokenTurn = game[lastNumberSpoken];
        game[lastNumberSpoken] = turn - 1;
        if (lastSpokenTurn >= 0)
            lastNumberSpoken = turn - 1 - lastSpokenTurn;
        else
            lastNumberSpoken = 0;
    }
    return lastNumberSpoken;
}

int main()
{
    const auto& startingNumbers = loadInput("day15.txt");
    std::cout << "part 1: " << solve(startingNumbers, 2020) << std::endl;
    std::cout << "part 2: " << solve(startingNumbers, 30000000) << std::endl;
}
