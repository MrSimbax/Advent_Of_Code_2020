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

constexpr Int N = 20201227ll;
constexpr Int X = 7;

struct Input
{
    Int pubKey1;
    Int pubKey2;
};

Input loadInput(const std::string& filename)
{
    std::ifstream ifs{filename};
    Input input{};
    ifs >> input.pubKey1;
    ifs >> input.pubKey2;
    return input;
}

Int fastModExp(Int x, Int a)
{
    if (a == 0) return 1;
    if (a == 1) return x % N;
    Int y = 1;
    while (a > 1)
    {
        if (a % 2 == 0)
        {
            x *= x;
            x %= N;
            a /= 2;
        }
        else
        {
            y *= x;
            y %= N;
            x *= x;
            x %= N;
            a = (a - 1) / 2;
        }
    }
    return (x * y) % N;
}

Int part1(const Input& input)
{
    Int acc = 1;
    Int a;
    for (a = 0; a < N - 1 && acc != input.pubKey1; ++a)
    {
        acc *= X;
        acc %= N;
    }
    return fastModExp(input.pubKey2, a);
}

int main()
{
    auto input = loadInput("day25.txt");
    std::cout << "part 1: " << part1(input) << std::endl;
}
