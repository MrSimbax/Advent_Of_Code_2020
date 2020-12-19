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

using Vec4D = std::array<Int, 4>;

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

template<typename T>
struct Array4D
{
    Array4D(Int xmin, Int ymin, Int zmin, Int wmin, Int xsize, Int ysize, Int zsize, Int wsize, T init = T()) :
            xmin{xmin},
            ymin{ymin},
            zmin{zmin},
            wmin{wmin},
            xsize{xsize},
            ysize{ysize},
            zsize{zsize},
            wsize{wsize},
            data(xsize * ysize * zsize * wsize, init)
    {
    }

    T& at(Int x, Int y, Int z, Int w)
    {
        return data[arrayIndex(x,y,z,w)];
    }

    T& operator()(Int x, Int y, Int z, Int w)
    {
        return at(x, y, z, w);
    }

    const T& at(Int x, Int y, Int z, Int w) const
    {
        return data[arrayIndex(x,y,z,w)];
    }

    const T& operator()(Int x, Int y, Int z, Int w) const
    {
        // std::cout << "(" << x << ", " << y << ")" << std::endl;
        return at(x, y, z, w);
    }

    bool isValidIndex(Int x, Int y, Int z, Int w) const
    {
        return 0 <= x && x < xsize && 0 <= y && y < ysize && 0 <= z && z < zsize && 0 <= w && w < wsize;
    }

    bool isValidCoord(Int x, Int y, Int z, Int w) const
    {
        return xmin <= x && x <= xmax() &&
        ymin <= y && y <= ymax() &&
        zmin <= z && z <= zmax() &&
        wmin <= w && w <= wmax();
    }

    Int xmax() const
    {
        return xmin + xsize - 1;
    }

    Int ymax() const
    {
        return ymin + ysize - 1;
    }

    Int zmax() const
    {
        return zmin + zsize - 1;
    }

    Int wmax() const
    {
        return wmin + wsize - 1;
    }

    Int xCoordToIndex(Int x) const
    {
        return x - xmin;
    }

    Int yCoordToIndex(Int y) const
    {
        return y - ymin;
    }

    Int zCoordToIndex(Int z) const
    {
        return z - zmin;
    }

    Int wCoordToIndex(Int w) const
    {
        return w - wmin;
    }

    Int arrayIndex(Int x, Int y, Int z, Int w) const
    {
        return wCoordToIndex(w) * xsize * ysize * zsize +
                zCoordToIndex(z) * xsize * ysize +
                yCoordToIndex(y) * xsize +
                xCoordToIndex(x);
    }

    std::vector<T> data{};
    Int xmin{0};
    Int ymin{0};
    Int zmin{0};
    Int wmin{0};
    Int xsize{0};
    Int ysize{0};
    Int zsize{0};
    Int wsize{0};
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

// returns a 3D map where
// the original map is placed so that the left top corner is at (0,0,0) and is in xy plane
// the 3d map has size increase in every direction
template<typename T>
Array4D<T> array2DTo4D(const Array2D<T>& array2D, Int sizeIncrease, T init = T())
{
    Array4D<T> array4D(-sizeIncrease, -sizeIncrease, -sizeIncrease, -sizeIncrease,
                    array2D.cols + 2 * sizeIncrease + 1, array2D.rows + 2 * sizeIncrease + 1, 2 * sizeIncrease + 1, 2 * sizeIncrease + 1,
                    init);
    for (Int x = 0; x < array2D.cols; ++x)
    {
        for (Int y = 0; y < array2D.rows; ++y)
        {
            array4D(x, y, 0, 0) = array2D(x, y);
        }
    }
    return array4D;
}

void printMap(const Array2D<char>& map)
{
    for (Int y = 0; y < map.rows; ++y)
    {
        for (Int x = 0; x < map.cols; ++x)
            std::cout << map(x,y);
        std::cout << std::endl;
    }
}

void printMap(const Array4D<char>& map)
{
    for (Int w = map.wmin; w < map.wmax(); ++w)
    {
        for (Int z = map.zmin; z < map.zmax(); ++z)
        {
            std::cout << "z = " << z << ", w = " << w << std::endl;
            for (Int y = map.ymin; y < map.ymax(); ++y)
            {
                for (Int x = map.xmin; x < map.xmax(); ++x)
                    std::cout << map(x, y, z, w);
                std::cout << std::endl;
            }
        }
    }
}

std::vector<Vec4D> getNeighbors(Int x, Int y, Int z, Int w, Array4D<char>& map)
{
    std::vector<Vec4D> neighbors;
    for (Int dx = -1; dx <= 1; ++dx)
    {
        for (Int dy = -1; dy <= 1; ++dy)
        {
            for (Int dz = -1; dz <= 1; ++dz)
            {
                for (Int dw = -1; dw <= 1; ++dw)
                {
                    Vec4D candidate{x + dx, y + dy, z + dz, w + dw};
                    if (candidate[0] == x && candidate[1] == y && candidate[2] == z && candidate[3] == w)
                        continue;
                    if (map.isValidCoord(candidate[0], candidate[1], candidate[2], candidate[3]))
                    {
                        neighbors.push_back(candidate);
                    }
                }
            }
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

void updateMap(Array4D<char>& map)
{
    auto oldMap = map;
    for (Int w = map.wmin; w < map.wmax(); ++w)
    {
        for (Int z = map.zmin; z < map.zmax(); ++z)
        {
            for (Int y = map.ymin; y < map.ymax(); ++y)
            {
                for (Int x = map.xmin; x < map.xmax(); ++x)
                {
                    char c = map(x, y, z, w);
                    auto neighbors = getNeighbors(x, y, z, w, oldMap);
                    auto numberOfActiveNeighbors =
                            std::count_if(neighbors.begin(), neighbors.end(),
                                          [&](const Vec4D& pos) { return oldMap(pos[0], pos[1], pos[2], pos[3]) == '#'; });
                    if (c == '#' && (numberOfActiveNeighbors != 2 && numberOfActiveNeighbors != 3))
                    {
                        map(x, y, z, w) = '.';
                    } else if (c == '.' && numberOfActiveNeighbors == 3)
                    {
                        map(x, y, z, w) = '#';
                    }
                }
            }
        }
    }
}

Int cycle(Array4D<char> map, Int cycleCount)
{
    auto prevMap = map;
    updateMap(map);
    for (Int cycle = 1; cycle < cycleCount; ++cycle)
    {
//        printMap(map);
//        std::cout << std::endl;

        prevMap = map;
        updateMap(map);
    }
    return std::count(map.data.begin(), map.data.end(), '#');
}

int main()
{
    auto map = array2DTo4D(loadMap("day17.txt"), 7, '.');
//    printMap(map);
    std::cout << "part 2: " << cycle(map, 6) << std::endl;
//    std::cout << "part 2: " << part2(map) << std::endl;
}
