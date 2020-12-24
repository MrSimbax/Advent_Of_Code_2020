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
using Vec2D = std::array<Int, 2>;

template<typename T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    static std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

struct Vec2DHasher
{
    std::size_t operator()(const Vec2D& vec) const
    {
        std::size_t seed = 0;
        hash_combine(seed, vec[0]);
        hash_combine(seed, vec[1]);
        return seed;
    }
};

// Self resizable hex grid with origin at (0,0)
// implemented with a hash map
// axial coordinates
//         +r  +q
//         |  /
//        / \
//       |   |
//        \ /
template<typename T>
class HexGrid
{
public:
    HexGrid() : data{} {}

    T& at(const Vec2D& pos)
    {
        return data[pos];
    }

    T& operator()(const Vec2D& pos)
    {
        return at(pos);
    }

    const T& at(const Vec2D& pos) const
    {
        return data.at(pos);
    }

    const T& operator()(const Vec2D& pos) const
    {
        return at(pos);
    }

    bool exists(const Vec2D& pos) const
    {
        return data.count(pos) > 0;
    }

    std::vector<Vec2D> getNeighborsPositions(const Vec2D& pos) const
    {
        return {{pos[0]+1,pos[1]+0}, // e
                {pos[0]-1,pos[1]+0}, // w
                {pos[0]+0,pos[1]-1}, // se
                {pos[0]-1,pos[1]-1}, // sw
                {pos[0]+1,pos[1]+1}, // ne
                {pos[0]+0,pos[1]+1}}; // nw
    }

    std::unordered_map<Vec2D, T, Vec2DHasher> data;
};

struct BadInput : std::exception {};

Vec2D loadHexPos(const std::string& line)
{
    std::istringstream iss{line};
    char c;
    Vec2D pos{0,0};
    while (iss >> c)
    {
        Vec2D step;
        char c2;
        switch (c)
        {
            case 'e': step = {1, 0}; break;
            case 'w': step = {-1, 0}; break;
            case 's':
                iss >> c2;
                switch (c2)
                {
                    case 'e': step = {0, -1}; break;
                    case 'w': step = {-1, -1}; break;
                    default: throw BadInput{};
                }
                break;
            case 'n':
                iss >> c2;
                switch (c2)
                {
                    case 'e': step = {1, 1}; break;
                    case 'w': step = {0, 1}; break;
                    default: throw BadInput{};
                }
                break;
            default: throw BadInput{};
        }
        pos[0] += step[0];
        pos[1] += step[1];
    }
    return pos;
}

void addNotExistingNeighbors(HexGrid<bool>& hexGrid, const Vec2D& pos)
{
    for (const auto& neighborPos : hexGrid.getNeighborsPositions(pos))
        if (!hexGrid.exists(neighborPos))
            hexGrid(neighborPos) = false;
}

// false = white
// true = black
HexGrid<bool> loadHexGrid(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::string line;
    HexGrid<bool> hexGrid;
    while (std::getline(ifs, line))
    {
        auto hexPos = loadHexPos(line);
        if (hexGrid.exists(hexPos))
            hexGrid(hexPos) = !hexGrid(hexPos);
        else
            hexGrid(hexPos) = true;
        addNotExistingNeighbors(hexGrid, hexPos);
    }
    return hexGrid;
}

Int countBlackTiles(const HexGrid<bool>& hexGrid)
{
    return std::count_if(hexGrid.data.begin(), hexGrid.data.end(),
                         [&](const std::map<Vec2D, bool>::value_type& p) { return p.second; });
}

Int countBlackNeighbors(const HexGrid<bool>& hexGrid, const Vec2D& pos)
{
    Int count = 0;
    for (const auto& neighborPos : hexGrid.getNeighborsPositions(pos))
        if (hexGrid.exists(neighborPos) && hexGrid(neighborPos))
            count += 1;
    return count;
}

Int part1(const HexGrid<bool>& hexGrid)
{
    return countBlackTiles(hexGrid);
}

void simulateDay(HexGrid<bool>& hexGrid)
{
    auto prevHexGrid = hexGrid;
    for (const auto& [pos, color] : prevHexGrid.data)
    {
        Int blackCount = countBlackNeighbors(prevHexGrid, pos);
        if (color) // black
        {
            if (blackCount == 0 || blackCount > 2)
                hexGrid(pos) = false;
        }
        else // white
        {
            if (blackCount == 2)
                hexGrid(pos) = true;
            addNotExistingNeighbors(hexGrid, pos);
        }
    }
}

void simulateDays(HexGrid<bool>& hexGrid, Int days)
{
    for (Int day = 1; day <= 100; ++day)
    {
        simulateDay(hexGrid);
//        std::cerr << "Day " << day << ": " << countBlackTiles(hexGrid) << std::endl;
    }
}

Int part2(HexGrid<bool>& hexGrid)
{
    simulateDays(hexGrid, 100);
    return countBlackTiles(hexGrid);
}

int main()
{
    auto hexGrid = loadHexGrid("day24.txt");
    std::cout << "part 1: " << part1(hexGrid) << std::endl;
    std::cout << "part 2: " << part2(hexGrid) << std::endl;
}

