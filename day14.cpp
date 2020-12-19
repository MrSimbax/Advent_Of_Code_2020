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
#include <memory>
#include <boost/algorithm/string.hpp>

using Int = long long int;

struct BadMask : public std::exception {};

Int applyMask(Int value, const std::string& mask)
{
    for (Int i = mask.size() - 1; i >= 0; --i)
    {
        char c = mask[i];
        Int j = mask.size() - 1 - i;
        switch (c)
        {
            case '0': value &= ~(static_cast<Int>(1) << j); break;
            case '1': value |= static_cast<Int>(1) << j; break;
            case 'X': break;
            default: throw BadMask{};
        }
    }
    return value;
}

struct Machine
{
    std::string mask;
    std::unordered_map<Int, Int> mem;
};

class Operation
{
public:
    virtual void run(Machine& machine) const = 0;
    ~Operation() = default;
};

class MaskAssignment : public Operation
{
public:
    explicit MaskAssignment(std::string newMask) : newMask{std::move(newMask)} {}

    void run(Machine& machine) const override
    {
        machine.mask = newMask;
    }

private:
    std::string newMask;
};

class MemAssignment : public Operation
{
public:
    MemAssignment(Int memIndex, Int newValue) : memIndex{memIndex}, newValue{newValue} {}

    void run(Machine& machine) const override
    {
        machine.mem[memIndex] = applyMask(newValue, machine.mask);
    }

private:
    Int memIndex;
    Int newValue;
};

std::vector<std::unique_ptr<Operation>> loadInput(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::string line;
    std::vector<std::unique_ptr<Operation>> operations;
    while (std::getline(ifs, line))
    {
        std::istringstream iss{line};
        std::string left;
        std::string op;
        std::string right;
        iss >> left >> op >> right;
        if (left == "mask")
        {
            operations.push_back(std::make_unique<MaskAssignment>(right));
        }
        else
        {
            std::vector<std::string> splitLeft;
            boost::split(splitLeft, line, boost::is_any_of("[]"));
            Int memIndex = std::stoll(splitLeft[1]);
            Int newValue = std::stoll(right);
            operations.push_back(std::make_unique<MemAssignment>(memIndex, newValue));
        }
    }
    return operations;
}

Machine runOperations(const std::vector<std::unique_ptr<Operation>>& operations)
{
    Machine machine{};
    for (const auto& operation : operations)
        operation->run(machine);
    return machine;
}

Int solve(const std::vector<std::unique_ptr<Operation>>& operations)
{
    const auto& machine = runOperations(operations);
    return std::accumulate(machine.mem.cbegin(), machine.mem.cend(), static_cast<Int>(0),
                           [](Int value, const std::unordered_map<Int, Int>::value_type& cell)
                           {
                                return value + cell.second;
                           });
}

void allValues(Int value, Int floatingPosIdx, const std::vector<Int>& floatingPositions, std::vector<Int>& values)
{
    if (floatingPosIdx >= floatingPositions.size())
    {
        values.push_back(value);
        return;
    }
    Int floatingPos = floatingPositions[floatingPosIdx];
    value &= ~(static_cast<Int>(1) << floatingPos); // apply 0 at floating pos
    allValues(value, floatingPosIdx + 1, floatingPositions, values);
    value |= static_cast<Int>(1) << floatingPos; // apply 1 at floating pos
    allValues(value, floatingPosIdx + 1, floatingPositions, values);
}

std::vector<Int> applyMaskV2(Int value, const std::string& mask)
{
    std::vector<Int> floatingPositions{};
    for (Int i = mask.size() - 1; i >= 0; --i)
    {
        char c = mask[i];
        Int j = mask.size() - 1 - i;
        switch (c)
        {
            case '0': break;
            case '1': value |= static_cast<Int>(1) << j; break;
            case 'X': floatingPositions.push_back(j); break;
            default: throw BadMask{};
        }
    }
    std::vector<Int> values;
    allValues(value, 0, floatingPositions, values);
    return values;
}

class MemAssignmentV2 : public Operation
{
public:
    MemAssignmentV2(Int memIndex, Int newValue) : memIndex{memIndex}, newValue{newValue} {}

    void run(Machine& machine) const override
    {
        const auto& addresses = applyMaskV2(memIndex, machine.mask);
        for (const auto& address : addresses)
            machine.mem[address] = newValue;
    }

private:
    Int memIndex;
    Int newValue;
};

std::vector<std::unique_ptr<Operation>> loadInputV2(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::string line;
    std::vector<std::unique_ptr<Operation>> operations;
    while (std::getline(ifs, line))
    {
        std::istringstream iss{line};
        std::string left;
        std::string op;
        std::string right;
        iss >> left >> op >> right;
        if (left == "mask")
        {
            operations.push_back(std::make_unique<MaskAssignment>(right));
        }
        else
        {
            std::vector<std::string> splitLeft;
            boost::split(splitLeft, line, boost::is_any_of("[]"));
            Int memIndex = std::stoll(splitLeft[1]);
            Int newValue = std::stoll(right);
            operations.push_back(std::make_unique<MemAssignmentV2>(memIndex, newValue));
        }
    }
    return operations;
}

int main()
{
    std::cout << "part 1: " << solve(loadInput("day14.txt")) << std::endl;
    std::cout << "part 2: " << solve(loadInputV2("day14.txt")) << std::endl;
}
