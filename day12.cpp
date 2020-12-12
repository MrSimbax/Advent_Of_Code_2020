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

using Vec2D = std::array<Int, 2>;

enum InstructionType
{
    north,
    south,
    east,
    west,
    forward,
    left,
    right
};

struct BadInstructionType : public std::exception {};

InstructionType charToInstructionType(char c)
{
    switch (c)
    {
        case 'N': return InstructionType::north;
        case 'S': return InstructionType::south;
        case 'E': return InstructionType::east;
        case 'W': return InstructionType::west;
        case 'F': return InstructionType::forward;
        case 'L': return InstructionType::left;
        case 'R': return InstructionType::right;
        default: throw BadInstructionType{};
    }
}

struct Instruction
{
    InstructionType type;
    Int number;
};

Instruction loadInstruction(const std::string& line)
{
    std::istringstream iss{line};
    char c;
    iss >> c;
    Int arg;
    iss >> arg;
    return Instruction{charToInstructionType(c), arg};
}

std::vector<Instruction> loadInstructions(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::vector<Instruction> instructions;
    std::string line;
    while (std::getline(ifs, line))
        instructions.push_back(loadInstruction(line));
    return instructions;
}

struct Ship
{
    Vec2D position{0,0};
    Vec2D waypoint{10, 1};
    Int orientation{0}; // facing east => orientation 0
};

void move(Vec2D& position, Int length, const Vec2D& direction)
{
    position[0] += length * direction[0];
    position[1] += length * direction[1];
}

struct BadOrientation : public std::exception {};

Vec2D orientationToDirection(Int orientation)
{
    switch (orientation)
    {
        case 0: return {1,0}; // east
        case 90: return {0,1}; // north
        case 180: return {-1,0}; // west
        case 270: return {0,-1}; // south
        default: throw BadOrientation{};
    }
}

void rotate(Int& orientation, Int degrees)
{
    orientation += degrees;
    if (orientation < 0)
        orientation += 360;
    else
        orientation %= 360;
}

void move(Ship& ship, const Instruction& instruction)
{
    switch (instruction.type)
    {
        case InstructionType::north:
            move(ship.position, instruction.number, {0,1});
            break;
        case InstructionType::south:
            move(ship.position, instruction.number, {0,-1});
            break;
        case InstructionType::east:
            move(ship.position, instruction.number, {1,0});
            break;
        case InstructionType::west:
            move(ship.position, instruction.number, {-1,0});
            break;
        case InstructionType::forward:
            move(ship.position,  instruction.number, orientationToDirection(ship.orientation));
            break;
        case InstructionType::left:
            rotate(ship.orientation, instruction.number);
            break;
        case InstructionType::right:
            rotate(ship.orientation, -instruction.number);
            break;
    }
}

void rotateWaypoint(Vec2D& waypoint, Int degrees)
{
    degrees = degrees >= 0 ? degrees : 360 + degrees;
    const Vec2D dir = orientationToDirection(degrees);
    Int x = waypoint[0] * dir[0] - waypoint[1] * dir[1];
    Int y = waypoint[0] * dir[1] + waypoint[1] * dir[0];
    waypoint[0] = x;
    waypoint[1] = y;
}

void moveWaypoint(Ship& ship, const Instruction& instruction)
{
    switch (instruction.type)
    {
        case InstructionType::north:
            move(ship.waypoint, instruction.number, {0,1});
            break;
        case InstructionType::south:
            move(ship.waypoint, instruction.number, {0,-1});
            break;
        case InstructionType::east:
            move(ship.waypoint, instruction.number, {1,0});
            break;
        case InstructionType::west:
            move(ship.waypoint, instruction.number, {-1,0});
            break;
        case InstructionType::forward:
            move(ship.position, instruction.number, ship.waypoint);
            break;
        case InstructionType::left:
            rotateWaypoint(ship.waypoint, instruction.number);
            break;
        case InstructionType::right:
            rotateWaypoint(ship.waypoint, -instruction.number);
            break;
    }
}

Int manhattanDistance(const Vec2D& from, const Vec2D& to)
{
    return std::abs(from[0] - to[0]) + std::abs(from[1] - to[1]);
}

Int part1(const std::vector<Instruction>& instructions)
{
    Ship ship{};
    for (const auto& instruction : instructions)
        move(ship, instruction);
    return manhattanDistance({0,0}, ship.position);
}

Int part2(const std::vector<Instruction>& instructions)
{
    Ship ship{};
    for (const auto& instruction : instructions)
        moveWaypoint(ship, instruction);
    return manhattanDistance({0,0}, ship.position);
}

int main()
{
    const auto& instructions = loadInstructions("day12.txt");
    std::cout << "part 1: " << part1(instructions) << std::endl;
    std::cout << "part 2: " << part2(instructions) << std::endl;
}
