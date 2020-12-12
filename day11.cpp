#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <array>
#include <numeric>

using Int = long long int;

using Vec2D = std::array<Int, 2>;

struct BadIndex : public std::exception {};

template<typename T>
struct Array2D
{
    T& at(Int x, Int y)
    {
        return data[y * cols + x];
    }

    T& operator()(Int x, Int y)
    {
        return at(x, y);
    }

    const T& at(Int x, Int y) const
    {
        return data[y * cols + x];
    }

    const T& operator()(Int x, Int y) const
    {
        // std::cout << "(" << x << ", " << y << ")" << std::endl;
        return at(x,y);
    }

    bool isValidIndex(Int x, Int y)
    {
        return 0 <= x && x < cols && 0 <= y && y < rows;
    }

    std::vector<T> data{};
    Int cols{0};
    Int rows{0};
};

Array2D<char> loadMap(const std::string& filename)
{
    std::ifstream ifs{filename};
    Array2D<char> map{};
    std::string line;
    while (getline(ifs, line))
    {
        for (char c : line)
            map.data.push_back(c);
        if (map.cols == 0)
            map.cols = line.size();
        ++map.rows;
    }
    return map;
}

void printMap(const Array2D<char>& map)
{
    for (Int y = 0; y < map.rows; ++y)
    {
        for (Int x = 0; x < map.cols; ++x)
        {
            std::cout << map(x,y);
        }
        std::cout << std::endl;
    }
}

std::vector<Vec2D> getNeighbors(Int x, Int y, Array2D<char>& map)
{
    std::vector<Vec2D> neighbors{
        {x - 1, y - 1},
        {x - 1, y},
        {x - 1, y + 1},
        {x, y - 1},
        {x, y + 1},
        {x + 1, y - 1},
        {x + 1, y},
        {x + 1, y + 1}
    };
    neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                    [&](const Vec2D& pos) { return !map.isValidIndex(pos[0],pos[1]); }), neighbors.end());
    // std::cout << "x = " << x << ", y = " << y << std::endl;
    // for (const auto& pos : neighbors)
    // {
    //     std::cout << pos[0] << ',' << pos[1] << std::endl;
    // }
    // std::cout << std::endl;
    return neighbors;
}

void updateMap(Array2D<char>& map)
{
    auto oldMap = map;
    for (Int y = 0; y < map.rows; ++y)
    {
        for (Int x = 0; x < map.cols; ++x)
        {
            char c = map(x,y);
            auto neighbors = getNeighbors(x,y,oldMap);
            if (c == 'L' &&
                std::all_of(neighbors.begin(), neighbors.end(),
                    [&](const Vec2D& pos) { return oldMap(pos[0],pos[1]) != '#'; }))
            {
                map(x,y) = '#';
            }
            else if (c == '#' &&
                std::count_if(neighbors.begin(), neighbors.end(),
                    [&](const Vec2D& pos) { return oldMap(pos[0],pos[1]) == '#'; }) >= 4)
            {
                map(x,y) = 'L';
            }
        }
    }
}

Int part1(Array2D<char> map)
{
    auto prevMap = map;
    updateMap(map);
    while (prevMap.data != map.data)
    {
        // printMap(map);
        // std::cout << std::endl;

        prevMap = map;
        updateMap(map);
    }
    return std::count(map.data.begin(), map.data.end(), '#');
}

std::vector<Vec2D> getNeighbors2(Int x, Int y, Array2D<char>& map)
{
    std::vector<Vec2D> steps{
        {-1, -1},
        {-1, 0},
        {-1, 1},
        {0, -1},
        {0, 1},
        {1, -1},
        {1, 0},
        {1, 1}
    };
    std::vector<Vec2D> neighbors{};
    for (const auto& step : steps)
    {
        Vec2D pos = {x + step[0],y + step[1]};
        while (map.isValidIndex(pos[0],pos[1]) && map(pos[0],pos[1]) == '.')
        {
            pos[0] += step[0];
            pos[1] += step[1];
        }
        if (map.isValidIndex(pos[0],pos[1]))
        {
            neighbors.push_back(pos);
        }
    }
    // std::cout << "x = " << x << ", y = " << y << std::endl;
    // for (const auto& pos : neighbors)
    // {
    //     std::cout << pos[0] << ',' << pos[1] << std::endl;
    // }
    // std::cout << std::endl;
    return neighbors;
}

void updateMap2(Array2D<char>& map)
{
    auto oldMap = map;
    for (Int y = 0; y < map.rows; ++y)
    {
        for (Int x = 0; x < map.cols; ++x)
        {
            char c = map(x,y);
            auto neighbors = getNeighbors2(x,y,oldMap);
            if (c == 'L' &&
                std::all_of(neighbors.begin(), neighbors.end(),
                    [&](const Vec2D& pos) { return oldMap(pos[0],pos[1]) != '#'; }))
            {
                map(x,y) = '#';
            }
            else if (c == '#' &&
                std::count_if(neighbors.begin(), neighbors.end(),
                    [&](const Vec2D& pos) { return oldMap(pos[0],pos[1]) == '#'; }) >= 5)
            {
                map(x,y) = 'L';
            }
        }
    }
}

Int part2(Array2D<char> map)
{
    auto prevMap = map;
    updateMap2(map);
    while (prevMap.data != map.data)
    {
        // printMap(map);
        // std::cout << std::endl;

        prevMap = map;
        updateMap2(map);
    }
    return std::count(map.data.begin(), map.data.end(), '#');
}

int main()
{
    auto map = loadMap("day11.txt");
    std::cout << "part 1: " << part1(map) << std::endl;
    std::cout << "part 2: " << part2(map) << std::endl;
}
