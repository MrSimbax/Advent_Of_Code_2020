#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>

using Int = long long int;

std::vector<std::string> loadBoardingPasses(const std::string& inputFilename)
{
    std::ifstream ifs{inputFilename};
    std::vector<std::string> boardingPasses;
    std::string line;
    while (std::getline(ifs, line))
    {
        boardingPasses.push_back(line);
    }
    return boardingPasses;
}

Int getSeatId(const std::string& boardingPass)
{
    Int rowBot = 0;
    Int rowTop = 127;
    Int colLeft = 0;
    Int colRight = 7;

    for (Int i = 0; i <= 6; ++i)
    {
        double rowMid = rowBot + static_cast<double>(rowTop - rowBot) / 2.0;
        char c = boardingPass[i];
        if (c == 'F')
            rowTop = std::floor(rowMid);
        else // c == 'B'
            rowBot = std::ceil(rowMid);
    }

    for (Int i = 7; i <= 9; ++i)
    {
        double colMid = colLeft + static_cast<double>(colRight - colLeft) / 2.0;
        char c = boardingPass[i];
        if (c == 'L')
            colRight = std::floor(colMid);
        else // c == 'B'
            colLeft = std::ceil(colMid);
    }

    Int rowId = rowBot;
    Int colId = colLeft;

    return rowId * 8 + colId;
}

Int part1(const std::string& filename)
{
    auto boardingPasses = loadBoardingPasses(filename);
    std::vector<Int> seatIds(boardingPasses.size());
    std::transform(boardingPasses.begin(), boardingPasses.end(), seatIds.begin(),
        [](const auto& boardingPass) { return getSeatId(boardingPass); });
    return *std::max_element(seatIds.begin(), seatIds.end());
}

Int part2(const std::string& filename)
{
    auto boardingPasses = loadBoardingPasses(filename);
    std::vector<Int> seatIds(boardingPasses.size());
    std::transform(boardingPasses.begin(), boardingPasses.end(), seatIds.begin(),
                   [](const auto& boardingPass) { return getSeatId(boardingPass); });
    std::sort(seatIds.begin(), seatIds.end());

    Int mySeatId = seatIds[0] + 1;
    for (auto it = seatIds.begin() + 1; it != seatIds.end() - 1; ++it)
    {
        if (mySeatId != *it)
            return mySeatId;
        ++mySeatId;
    }
    return -1;
}

int main()
{
    std::cout << "part 1: " << part1("day5.txt") << std::endl;
    std::cout << "part 2: " << part2("day5.txt") << std::endl;
}
