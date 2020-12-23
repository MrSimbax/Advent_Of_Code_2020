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
#include <memory>
#include <stack>
#include <chrono>

using Int = long long int;
using Cups = std::vector<Int>; // cups[i] = j means cup j is after i

std::ostream& printCups(std::ostream& os, const Cups& cups, Int from = 1)
{
    Int cup = from;
    do
    {
        os << cup;
        cup = cups[cup];
    } while (cup != from);
    return os;
}

std::vector<Int> loadLabels(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::vector<Int> labels;
    char c;
    while (ifs >> c)
        labels.push_back(c - '0');
    return labels;
}

Cups makeCups(const std::vector<Int>& labels)
{
    Cups cups(labels.size() + 1, 0);
    for (Int i = 0; i < labels.size() - 1; ++i)
    {
        auto cup = labels[i];
        auto nextCup = labels[i+1];
        cups[cup] = nextCup;
    }
    cups[labels[labels.size() - 1]] = labels[0];
    return cups;
}

Int makeMove(Int cup, Cups& cups)
{
//    std::cerr << "current cup: " << cup << std::endl;
//    std::cerr << "cups: ";
//    printCups(std::cerr, cups, cup);
//    std::cerr << std::endl;

    // Grab three next cups
    Int cup1 = cups[cup];
    Int cup2 = cups[cup1];
    Int cup3 = cups[cup2];
    cups[cup] = cups[cup3];

//    std::cerr << "pick up: " << cup1 << " " << cup2 << " " << cup3 << std::endl;

    // Find destination cup label
    auto destCup = cup;
    do
    {
        --destCup;
        if (destCup <= 0)
            destCup = cups.size() - 1;
    } while (destCup == cup1 || destCup == cup2 || destCup == cup3);

//    std::cerr << "dest: " << destCup << std::endl;

    // Place the taken cups after destination cup
    cups[cup3] = cups[destCup];
    cups[destCup] = cup1;

    return cups[cup];
}

void playGame(Int firstCup, Cups& cups, Int numberOfMoves)
{
    auto currentCup = firstCup;
    for (Int i = 1; i <= numberOfMoves; ++i)
    {
//        std::cerr << "-- move " << i << " --" << std::endl;
        currentCup = makeMove(currentCup, cups);
//        std::cerr << std::endl;
    }
}

std::string part1(const std::vector<Int>& labels)
{
    auto cups = makeCups(labels);
    playGame(labels[0], cups, 100);
    std::ostringstream oss;
    printCups(oss, cups, 1);
    return oss.str();
}

std::vector<Int> labelsWithMoreCups(const std::vector<Int>& labels, Int limit)
{
    auto ret = labels;
    for (Int i = labels.size() + 1; i <= limit; ++i)
        ret.push_back(i);
    return ret;
}

Int part2(const std::vector<Int>& p_labels)
{
    auto labels = labelsWithMoreCups(p_labels, 1000000);
    auto cups = makeCups(labels);
    playGame(labels[0], cups, 10000000);
    return cups[1] * cups[cups[1]];
}

int main()
{
    auto labels = loadLabels("day23.txt");
    std::cout << "part 1: " << part1(labels) << std::endl;
    std::cout << "part 2: " << part2(labels) << std::endl;
}
