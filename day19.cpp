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

struct ValidatorResult
{
    std::string::const_iterator newMsgBegin;
    bool isMessageValid;
};

struct Rule
{
    Int index;
    std::vector<std::vector<Int>> subrules;
    char character{0};

    ValidatorResult isMessageValid(std::string::const_iterator msgBegin, std::string::const_iterator msgEnd,
                                   const std::vector<Rule>& rules) const
    {
        if (msgBegin == msgEnd)
            return {msgEnd, false};
        if (!subrules.empty())
        {
            for (const auto& subrule : subrules)
            {
                bool subruleValidationSuccessful = true;
                std::string::const_iterator it = msgBegin;
                for (const auto rule : subrule)
                {
                    const auto& result = rules[rule].isMessageValid(it, msgEnd, rules);
                    if (!result.isMessageValid)
                    {
                        subruleValidationSuccessful = false;
                        break;
                    }
                    else
                    {
                        it = result.newMsgBegin;
                    }
                }
                if (subruleValidationSuccessful)
                {
                    return {it, subruleValidationSuccessful};
                }
            }
            return {msgEnd, false};
        }
        else
        {
            return {msgBegin + 1, *msgBegin == character};
        }
    }
};

Rule loadRule(const std::string& line)
{
    Rule rule{};
    std::istringstream iss{line};
    iss >> rule.index;
    std::string str;
    iss >> str; // :
    std::vector<Int> subrule;
    while (iss >> str)
    {
        if (str == "|")
        {
            rule.subrules.push_back(subrule);
            subrule.clear();
        }
        else if (str[0] == '"')
        {
            rule.character = str[1];
        }
        else
        {
            subrule.push_back(std::stoll(str));
        }
    }
    if (!subrule.empty())
    {
        rule.subrules.push_back(subrule);
    }
    return rule;
}

struct Input
{
    std::vector<Rule> rules;
    std::vector<std::string> messages;
};

Input loadInput(const std::string& filename)
{
    std::ifstream ifs{filename};
    Input input{};
    std::string line;
    while (std::getline(ifs, line) && !line.empty())
        input.rules.push_back(loadRule(line));
    std::sort(input.rules.begin(), input.rules.end(),
              [](const Rule& a, const Rule& b) { return a.index < b.index; });
    while (std::getline(ifs, line))
        input.messages.push_back(line);
    return input;
}

Int part1(const Input& input)
{
    return std::count_if(input.messages.begin(), input.messages.end(),
                         [&input](const std::string& msg)
                         {
                            const auto& result = input.rules[0].isMessageValid(msg.begin(), msg.end(), input.rules);
                            return result.isMessageValid && result.newMsgBegin == msg.end();
                         });
}

int main()
{
    auto input = loadInput("day19.txt");
    std::cout << "part 1: " << part1(input) << std::endl;
}
