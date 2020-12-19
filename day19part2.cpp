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
#include <boost/algorithm/string.hpp>

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

    std::unordered_set<std::string> getPossibleMessages(const std::vector<Rule>& rules) const
    {
        if (subrules.empty())
        {
            return {std::string{character}};
        }
        else
        {
            std::unordered_set<std::string> retSet;
            for (const auto& subrule : subrules)
            {
                if (subrule.size() == 1)
                {
                    auto set1 = rules[subrule[0]].getPossibleMessages(rules);
                    for (const auto& str1 : set1)
                    {
                        retSet.insert(str1);
                    }
                }
                else if (subrule.size() == 2)
                {
                    auto set1 = rules[subrule[0]].getPossibleMessages(rules);
                    auto set2 = rules[subrule[1]].getPossibleMessages(rules);
                    for (const auto& str1 : set1)
                    {
                        for (const auto& str2 : set2)
                        {
                            retSet.insert(str1 + str2);
                        }
                    }
                }
                else
                {
                    throw std::runtime_error{"bad assumption"};
                }
            }
            return retSet;
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

    if (rule.index == 8)
    {
        rule.subrules.push_back({42, 8});
    }
    else if (rule.index == 11)
    {
        rule.subrules.push_back({42, 11, 31});
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

ValidatorResult isMessageValid(
        std::string::const_iterator msgBegin,
        const std::unordered_set<std::string>& set)
{
    for (const auto& str : set)
    {
        bool match = true;
        for (Int i = 0; i < str.size(); ++i)
        {
            if (*(msgBegin + i) != str[i])
            {
                match = false;
            }
        }
        if (match)
            return {msgBegin + str.size(), true};
    }
    return {msgBegin, false};
}

Int part2(const Input& input)
{
    // 0 -> 8 11
    // 8 -> 42 | 42 8
    // 11 -> 42 31 | 42 11 31
    // hence
    // 0 -> 42{x}42{y}31{y}, x,y>0
    // or
    // 0 -> 42{x}31{y}, x>y>0
    // rules 8 and 11 are not used anywhere else
    const auto& rule42 = input.rules[42];
    const auto& rule31 = input.rules[31];
    auto set42 = rule42.getPossibleMessages(input.rules);
    auto set31 = rule31.getPossibleMessages(input.rules);
    Int validMsgsCount = 0;
    for (const auto& msg : input.messages)
    {
        Int rule42Count = 0;
        Int rule31Count = 0;
        auto it = msg.begin();
        // count 42s
        while (it != msg.end())
        {
            const auto& result = isMessageValid(it, set42);
            if (result.isMessageValid)
            {
                it = result.newMsgBegin;
                ++rule42Count;
            }
            else
            {
                break;
            }
        }
        if (rule42Count < 2)
            continue;
        while (it != msg.end())
        {
            const auto& result = isMessageValid(it, set31);
            if (result.isMessageValid)
            {
                it = result.newMsgBegin;
                ++rule31Count;
            }
            else
            {
                break;
            }
        }
        if (rule31Count < 1)
            continue;
        if (it == msg.end() && rule42Count > rule31Count)
        {
            ++validMsgsCount;
        }
    }
    return validMsgsCount;
}

int main()
{
    auto input = loadInput("day19.txt");
    std::cout << "part 2: " << part2(input) << std::endl;
}
