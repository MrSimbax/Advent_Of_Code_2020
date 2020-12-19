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

using Int = long long int;

char readToken(const std::string& line, std::string::const_iterator& it)
{
    while (it != line.end() && *it == ' ')
        ++it;
    if (it != line.end())
    {
        char c = *it;
        ++it;
        return c; // +,-,(,),digit
    }
    return 0;
}

// part 1
// Grammar:
// start -> expression end
// expression -> expression + primary
// expression -> expression * primary
// expression -> primary
// primary -> number
// primary -> '(' expression ')'
Int expression(const std::string& line, std::string::const_iterator& it);

Int primary(const std::string& line, std::string::const_iterator& it)
{
    char c = readToken(line,it);
    if (isdigit(c))
        return c - '0';
    else if (c == '(')
    {
        Int expr = expression(line, it);
        readToken(line, it); // ')'
        return expr;
    }
}

Int expression(const std::string& line, std::string::const_iterator& it)
{
    Int left = primary(line, it);
    while (true)
    {
        char operationKind = readToken(line, it);
        switch (operationKind)
        {
            case '+': left += primary(line, it); break;
            case '*': left *= primary(line, it); break;
            default: --it; return left;
        }
    }
}

Int calculate(const std::string& line)
{
    auto it = line.begin();
    return expression(line, it);
}

// part 2
// Grammar:
// start -> expression end
// expression -> expression * term
// expression -> term
// term -> expression + primary
// term -> primary
// primary -> number
// primary -> '(' expression ')'
Int expression2(const std::string& line, std::string::const_iterator& it);

Int primary2(const std::string& line, std::string::const_iterator& it)
{
    char c = readToken(line,it);
    if (isdigit(c))
        return c - '0';
    else if (c == '(')
    {
        Int expr = expression2(line, it);
        readToken(line, it); // ')'
        return expr;
    }
}

Int term(const std::string& line, std::string::const_iterator& it)
{
    Int left = primary2(line, it);
    while (true)
    {
        char operationKind = readToken(line, it);
        switch (operationKind)
        {
            case '+': left += primary2(line, it); break;
            default: --it; return left;
        }
    }
}

Int expression2(const std::string& line, std::string::const_iterator& it)
{
    Int left = term(line, it);
    while (true)
    {
        char operationKind = readToken(line, it);
        switch (operationKind)
        {
            case '*': left *= term(line, it); break;
            default: --it; return left;
        }
    }
}

Int calculate2(const std::string& line)
{
    auto it = line.begin();
    return expression2(line, it);
}

// ---
std::vector<std::string> loadInput(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::vector<std::string> homework;
    std::string line;
    while (std::getline(ifs, line))
        homework.push_back(line);
    return homework;
}

Int part1(const std::vector<std::string>& lines)
{
    return std::accumulate(lines.begin(), lines.end(), static_cast<Int>(0),
                           [](Int a, const std::string& line) { return a + calculate(line); });
}

Int part2(const std::vector<std::string>& lines)
{
    return std::accumulate(lines.begin(), lines.end(), static_cast<Int>(0),
                           [](Int a, const std::string& line) { return a + calculate2(line); });
}

int main()
{
    std::cout << calculate2("5 + 8 * ((6 * 5 * 8 + 5 + 8) * 7 + 3)") << std::endl;
    // 23062

    auto input = loadInput("day18.txt");
    std::cout << "part 1: " << part1(input) << std::endl;
    std::cout << "part 2: " << part2(input) << std::endl;
}
