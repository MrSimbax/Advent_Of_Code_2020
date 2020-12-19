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

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    os << "{";
    auto it = vec.cbegin();
    for (; it != vec.cend() && it != vec.cend() - 1; ++it)
    {
        os << *it << ", ";
    }
    if (it != vec.cend())
    {
        os << *it;
    }
    os << "}";
    return os;
}

struct Rule
{
    std::string fieldName;
    Int range1Min;
    Int range1Max;
    Int range2Min;
    Int range2Max;

    [[nodiscard]] bool isValid(Int value) const
    {
        return (range1Min <= value && value <= range1Max) ||
               (range2Min <= value && value <= range2Max);
    }
};

using Rules = std::unordered_map<std::string, Rule>;
using Ticket = std::vector<Int>;

struct Input
{
    Rules rules;
    Ticket myTicket;
    std::vector<Ticket> nearbyTickets;
};

Rule loadRule(const std::string& line)
{
    Rule rule{};
    std::vector<std::string> split;
    boost::split(split, line, boost::is_any_of(":"));
    rule.fieldName = split[0];
    std::istringstream iss{split[1]};

    std::string skipstr;
    char skipc;

    iss >> rule.range1Min;
    iss >> skipc; // -
    iss >> rule.range1Max;

    iss >> skipstr; // or

    iss >> rule.range2Min;
    iss >> skipc; // -
    iss >> rule.range2Max;

    return rule;
}

Ticket loadTicket(const std::string& line)
{
    Ticket ticket;
    std::vector<std::string> split;
    boost::split(split, line, boost::is_any_of(","));
    std::transform(split.begin(), split.end(), std::back_inserter(ticket),
                   [](const std::string& str) { return std::stoll(str); });
    return ticket;
}

Input loadInput(const std::string& inputFilename)
{
    Input input{};

    std::ifstream ifs{inputFilename};
    std::string line;
    while (std::getline(ifs, line) && !line.empty())
    {
        Rule rule = loadRule(line);
        input.rules[rule.fieldName] = rule;
    }

    std::getline(ifs, line); // your ticket
    std::getline(ifs, line);
    input.myTicket = loadTicket(line);

    std::getline(ifs, line); // empty line
    std::getline(ifs, line); // nearby tickets
    while (std::getline(ifs, line) && !line.empty())
    {
        input.nearbyTickets.push_back(loadTicket(line));
    }

    return input;
}

Int part1(Input& input)
{
    Int errorRate = 0;
    std::vector<Ticket> validNearbyTickets;
    for (const auto& nearbyTicket : input.nearbyTickets)
    {
        bool isValidTicket = true;
        for (const auto fieldValue : nearbyTicket)
        {
            if (!std::any_of(input.rules.begin(), input.rules.end(),
                [fieldValue](const Rules::value_type& p)
                {
                    const auto& rule = p.second;
                    return rule.isValid(fieldValue);
                }))
            {
                errorRate += fieldValue;
                isValidTicket = false;
            }
        }
        if (isValidTicket)
        {
            validNearbyTickets.push_back(nearbyTicket);
        }
    }
    input.nearbyTickets = validNearbyTickets;
    return errorRate;
}

Int part2(Input& input)
{
    // find sets of all possible fields for each field index
    std::vector<std::unordered_set<std::string>> possibleFields;
    Int numberOfFields = input.myTicket.size();
    for (Int fieldIndex = 0; fieldIndex < numberOfFields; ++fieldIndex)
    {
        possibleFields.emplace_back();
        for (const auto& rulePair : input.rules)
        {
            const auto& rule = rulePair.second;
            if (std::all_of(input.nearbyTickets.begin(), input.nearbyTickets.end(),
                             [&rule,fieldIndex](const Ticket& ticket)
                             {
                                 return rule.isValid(ticket[fieldIndex]);
                             }))
            {
                possibleFields[fieldIndex].insert(rule.fieldName);
            }
        }
    }

    // figure out the fields
    std::vector<std::string> fieldsInOrder(numberOfFields);
    Int numberOfSureFields = 0;

    while (numberOfSureFields < numberOfFields)
    {
        auto sureFieldIt = std::find_if(possibleFields.begin(), possibleFields.end(),
                                        [](const std::unordered_set<std::string>& fields)
                                        {
                                            return fields.size() == 1;
                                        });
        // boldly assume a set with size 1 exists
        auto sureFieldIdx = std::distance(possibleFields.begin(), sureFieldIt);
        const auto sureFieldName = *sureFieldIt->begin();

        // remove the sure field from other sets
        for (auto& fields : possibleFields)
            fields.erase(sureFieldName);

        fieldsInOrder[sureFieldIdx] = sureFieldName;
        ++numberOfSureFields;
    }

    std::cerr << fieldsInOrder << std::endl;

    // find the answer
    Int mult = 1;
    for (Int fieldIndex = 0; fieldIndex < numberOfFields; ++fieldIndex)
    {
        const auto& fieldName = fieldsInOrder[fieldIndex];
        if (boost::starts_with(fieldName, "departure"))
        {
            mult *= input.myTicket[fieldIndex];
        }
    }
    return mult;
}

int main()
{
    auto input = loadInput("day16.txt");
    std::cout << "part 1: " << part1(input) << std::endl;
    std::cout << "part 2: " << part2(input) << std::endl;
}
