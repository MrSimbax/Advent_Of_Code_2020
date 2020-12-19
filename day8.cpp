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

using Int = long long int;

enum class InstructionType
{
    nop,
    acc,
    jmp
};

struct Instruction
{
    InstructionType type;
    Int offset;
};

using CodeType = std::vector<Instruction>;

struct BadInstruction : public std::exception
{
};

InstructionType parseInstructionType(const std::string& str)
{
    if (str == "nop") return InstructionType::nop;
    if (str == "acc") return InstructionType::acc;
    if (str == "jmp") return InstructionType::jmp;
    else throw BadInstruction{};
}

Instruction parseInstruction(const std::string& line)
{
    std::istringstream iss{line};

    std::string instructionName;
    iss >> instructionName;

    Int offset;
    iss >> offset;

    return Instruction{parseInstructionType(instructionName), offset};
}

CodeType loadCode(const std::string& filename)
{
    CodeType code{};
    std::ifstream ifs{filename};
    std::string line;
    while (std::getline(ifs, line))
    {
        code.push_back(parseInstruction(line));
    }
    return code;
}

struct ExitStatus
{
    Int acc;
    bool loop;
};

ExitStatus runCode(const CodeType& code)
{
    std::vector<bool> instructionVisited(code.size(), false);
    Int acc = 0;
    Int ip = 0;
    while (true)
    {
        if (ip >= code.size())
        {
            return {acc,false};
        }

        if (instructionVisited[ip])
        {
            return {acc,true};;
        }

        instructionVisited[ip] = true;
        const auto& instruction = code[ip];
        switch (instruction.type)
        {
            case InstructionType::nop:
                ip += 1;
                break;

            case InstructionType::acc:
                acc += instruction.offset;
                ip += 1;
                break;

            case InstructionType::jmp:
                ip += instruction.offset;
                break;
        }
    }
}

Int part2(const CodeType& originalCode)
{
    CodeType code = originalCode;
    for (auto& instruction : code)
    {
        if (instruction.type != InstructionType::nop && instruction.type != InstructionType::jmp)
            continue;

        if (instruction.type == InstructionType::nop)
            instruction.type = InstructionType::jmp;
        else if (instruction.type == InstructionType::jmp)
            instruction.type = InstructionType::nop;

        auto exitStatus = runCode(code);
        if (!exitStatus.loop)
            return exitStatus.acc;

        if (instruction.type == InstructionType::nop)
            instruction.type = InstructionType::jmp;
        else if (instruction.type == InstructionType::jmp)
            instruction.type = InstructionType::nop;
    }
    return -1;
}

int main()
{
    std::cout << "part 1: " << runCode(loadCode("day8.txt")).acc << std::endl;
    std::cout << "part 2: " << part2(loadCode("day8.txt")) << std::endl;
}
