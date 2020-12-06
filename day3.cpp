#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <numeric>

#include <Eigen/Dense>

using Int = long long int;
using MapT = Eigen::Matrix<char, Eigen::Dynamic, Eigen::Dynamic>;
using Vec2 = Eigen::Matrix<Int, 2, 1>;

MapT loadMap(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::string row;
    std::getline(ifs, row);

    MapT map(1, row.size());
    for (Int x = 0; x < map.cols(); ++x)
    {
        map(0, x) = row[x];
    }

    Int y = 1;
    while (std::getline(ifs, row))
    {
        map.conservativeResize(y + 1, Eigen::NoChange_t{});
        for (Int x = 0; x < map.cols(); ++x)
        {
            map(y, x) = row[x];
        }
        ++y;
    }

    return map;
}

void printMap(const MapT& map)
{
    for (Int y = 0; y < map.rows(); ++y)
    {
        for (Int x = 0; x < map.cols(); ++x)
        {
            std::cout << map(y,x);
        }
        std::cout << '\n';
    }
}

Int countTrees(const MapT& map, const Vec2& step)
{
    Vec2 pos{0, 0};
    Int treesCount = 0;
    while (pos.y() < map.rows())
    {
        if (map(pos.y(), pos.x()) == '#')
        {
            ++treesCount;
        }
        pos.x() = (pos.x() + step.x()) % map.cols();
        pos.y() += step.y();
    }
    return treesCount;
}

Int countTrees(const MapT& map, const std::vector<Vec2>& steps)
{
    std::vector<Int> counts;
    counts.reserve(steps.size());
    for (const auto& step : steps)
    {
        counts.push_back(countTrees(map, step));
    }
    return std::accumulate(counts.begin(), counts.end(), static_cast<Int>(1), std::multiplies<Int>{});
}

int main()
{
    std::cout << "part 1: " << countTrees(loadMap("day3.txt"), Vec2{3, 1}) << std::endl;
    std::cout << "part 2: " << countTrees(loadMap("day3.txt"),
                                                {
                                                {1, 1},
                                                {3,1},
                                                {5,1},
                                                {7,1},
                                                {1,2}
                                                }) << std::endl;
}
