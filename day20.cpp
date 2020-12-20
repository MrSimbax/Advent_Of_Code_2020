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
#include <unordered_map>
#include <queue>

using Int = long long int;

using Vec2D = std::array<Int, 2>;

struct BadIndex : public std::exception {};

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

    bool isValidIndex(Int x, Int y) const
    {
        return 0 <= x && x < cols && 0 <= y && y < rows;
    }

    Array2D<T> rotatedCCW() const
    {
        Array2D<T> ret = *this;
        for (Int j = 0; j < cols; ++j)
            for (Int i = 0; i < rows; ++i)
                ret(i, cols - j - 1) = at(j, i);
        return ret;
    }

    Array2D<T> flippedV() const
    {
        Array2D<T> ret = *this;
        for (Int j = 0; j < cols; ++j)
            for (Int i = 0; i < rows; ++i)
                ret(j, rows - i - 1) = at(j, i);
        return ret;
    }

    std::vector<T> data{};
    Int cols{0};
    Int rows{0};
};

Array2D<char> loadArray2D(std::istream& ifs)
{
    Array2D<char> array2D{};
    std::string line;
    while (getline(ifs, line) && !line.empty())
    {
        for (char c : line)
            array2D.data.push_back(c);
        if (array2D.cols == 0)
            array2D.cols = line.size();
        ++array2D.rows;
    }
    return array2D;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const Array2D<T>& array2D)
{
    for (Int y = 0; y < array2D.rows; ++y)
    {
        for (Int x = 0; x < array2D.cols; ++x)
        {
            os << array2D(x, y);
        }
        os << std::endl;
    }
    return os;
}

// Self resizable grid 2D with origin at (0,0)
// ordered map implementation for simplicity
template<typename T>
class Grid2D
{
public:
    Grid2D() : data{} {}

    T& at(Int x, Int y)
    {
        return data[Vec2D{x, y}];
    }

    T& operator()(Int x, Int y)
    {
        return at(x, y);
    }

    const T& at(Int x, Int y) const
    {
        return data.at(Vec2D{x, y});
    }

    const T& operator()(Int x, Int y) const
    {
        return at(x,y);
    }

    bool exists(Int x, Int y) const
    {
        return data.count(Vec2D{x,y}) > 0;
    }

    template<typename Pred>
    std::vector<Vec2D> getNeighborsPositions(Int x, Int y, Pred pred)
    {
        std::vector<Vec2D> neighbors = {
            {x + 1, y},
            {x, y + 1},
            {x - 1, y},
            {x, y - 1}
        };
        std::vector<Vec2D> ret;
        for (const auto& neighbor : neighbors)
            if (pred(neighbor[0], neighbor[1]))
                ret.push_back(neighbor);
        return ret;
    }

    std::vector<Vec2D> getAllNeighborsPositions(Int x, Int y)
    {
        return getNeighborsPositions(x, y, [](Int x1, Int y1) { return true; });
    }

    std::vector<Vec2D> getExistingNeighborsPositions(Int x, Int y)
    {
        return getNeighborsPositions(x, y, [this](Int x1, Int y1) { return exists(x1, y1); });
    }

    std::vector<Vec2D> getNotExistingNeighborsPositions(Int x, Int y)
    {
        return getNeighborsPositions(x, y, [this](Int x1, Int y1) { return !exists(x1, y1); });
    }

private:
    std::map<Vec2D, T> data;
};

struct Tile
{
    Array2D<char> data;
    Int id{};
};

std::vector<Tile> loadTiles(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::string line;
    std::vector<Tile> tiles;
    while (std::getline(ifs, line))
    {
        Tile tile{};
        std::istringstream iss{line};
        std::string skip;
        iss >> skip;
        iss >> tile.id;
        tile.data = loadArray2D(ifs);
        tiles.push_back(tile);
    }
    return tiles;
}

std::vector<Tile> getAllVariantsOfTile(const Tile& p_tile)
{
    std::vector<Tile> ret;
    for (Int f = 0; f <= 1; ++f)
    {
        Tile tile = p_tile;
        if (f)
            tile.data = tile.data.flippedV();
        for (Int r = 0; r <= 3; ++r)
        {
            ret.push_back(tile);
            tile.data = tile.data.rotatedCCW();
        }
    }
    return ret;
}

template<typename T>
std::vector<Array2D<T>> getAllVariantsOfMap(const Array2D<T>& p_map)
{
    std::vector<Array2D<T>> ret;
    for (Int f = 0; f <= 1; ++f)
    {
        Array2D<T> map = p_map;
        if (f)
            map = map.flippedV();
        for (Int r = 0; r <= 3; ++r)
        {
            ret.push_back(map);
            map = map.rotatedCCW();
        }
    }
    return ret;
}

bool tryToPlaceTileAtPosition(Vec2D position, Grid2D<Tile>& puzzle, std::vector<Tile>& tiles)
{
    const auto& existingNeighbors = puzzle.getExistingNeighborsPositions(position[0], position[1]);
    if (existingNeighbors.empty())
    {
        Tile& tile = tiles.back();
        tiles.pop_back();
        puzzle(position[0], position[1]) = tile;
        return true;
    }
    else
    {
        for (Int tileIndex = 0; tileIndex < tiles.size(); ++tileIndex)
        {
            const auto& tile = tiles[tileIndex];
            for (const auto& candidate : getAllVariantsOfTile(tile))
            {
                const Vec2D rightNeighbor = {position[0] + 1, position[1]};
                const Vec2D upNeighbor = {position[0], position[1] + 1};
                const Vec2D leftNeighbor = {position[0] - 1, position[1]};
                const Vec2D downNeighbor = {position[0], position[1] - 1};
                bool rightNeighborMatches = true;
                bool upNeighborMatches = true;
                bool leftNeighborMatches = true;
                bool downNeighborMatches = true;
                if (puzzle.exists(rightNeighbor[0], rightNeighbor[1]))
                {
                    const Tile& neighbor = puzzle(rightNeighbor[0], rightNeighbor[1]);
                    Int x1 = candidate.data.cols - 1;
                    Int x2 = 0;
                    for (Int y = 0; y < candidate.data.rows; ++y)
                    {
                        if (candidate.data(x1,y) != neighbor.data(x2,y))
                        {
                            rightNeighborMatches = false;
                            break;
                        }
                    }
                }
                if (puzzle.exists(upNeighbor[0], upNeighbor[1])
                    && rightNeighborMatches)
                {
                    const Tile& neighbor = puzzle(upNeighbor[0], upNeighbor[1]);
                    Int y1 = 0;
                    Int y2 = candidate.data.rows - 1;
                    for (Int x = 0; x < candidate.data.cols; ++x)
                    {
                        if (candidate.data(x,y1) != neighbor.data(x,y2))
                        {
                            upNeighborMatches = false;
                            break;
                        }
                    }
                }
                if (puzzle.exists(leftNeighbor[0], leftNeighbor[1]) &&
                    rightNeighborMatches && upNeighborMatches)
                {
                    const Tile& neighbor = puzzle(leftNeighbor[0], leftNeighbor[1]);
                    Int x1 = 0;
                    Int x2 = candidate.data.cols - 1;
                    for (Int y = 0; y < candidate.data.rows; ++y)
                    {
                        if (candidate.data(x1,y) != neighbor.data(x2,y))
                        {
                            leftNeighborMatches = false;
                            break;
                        }
                    }
                }
                if (puzzle.exists(downNeighbor[0], downNeighbor[1]) &&
                    rightNeighborMatches && upNeighborMatches && leftNeighborMatches)
                {
                    const Tile& neighbor = puzzle(downNeighbor[0], downNeighbor[1]);
                    Int y1 = candidate.data.rows - 1;
                    Int y2 = 0;
                    for (Int x = 0; x < candidate.data.cols; ++x)
                    {
                        if (candidate.data(x,y1) != neighbor.data(x,y2))
                        {
                            downNeighborMatches = false;
                            break;
                        }
                    }
                }
                if (rightNeighborMatches && upNeighborMatches && leftNeighborMatches && downNeighborMatches)
                {
                    puzzle(position[0], position[1]) = candidate;
                    tiles.erase(tiles.begin() + tileIndex);
                    return true;
                }
            }
        }
        return false;
    }
}

Vec2D findUpperLeftCorner(const Grid2D<Tile>& puzzle)
{
    Vec2D upperLeftPos{0,0};
    while (puzzle.exists(upperLeftPos[0] - 1, upperLeftPos[1]))
        upperLeftPos[0] -= 1;
    while (puzzle.exists(upperLeftPos[0], upperLeftPos[1] + 1))
        upperLeftPos[1] += 1;
    return upperLeftPos;
}

Vec2D findBottomRightCorner(const Grid2D<Tile>& puzzle)
{
    Vec2D bottomRightPos{0,0};
    while (puzzle.exists(bottomRightPos[0] + 1, bottomRightPos[1]))
        bottomRightPos[0] += 1;
    while (puzzle.exists(bottomRightPos[0], bottomRightPos[1] - 1))
        bottomRightPos[1] -= 1;
    return bottomRightPos;
}

Array2D<char> convertGrid2DToArray2DWithGapsAndBordersRemoved(const Grid2D<Tile>& puzzle)
{
    Vec2D upperLeftPos = findUpperLeftCorner(puzzle);
    Vec2D bottomRightPos = findBottomRightCorner(puzzle);

    // Construct map
    const Tile& upperLeftTile = puzzle(upperLeftPos[0], upperLeftPos[1]);
    Array2D<char> map;
    const Int numberOfTilesX = bottomRightPos[0] - upperLeftPos[0] + 1;
    const Int numberOfTilesY = upperLeftPos[1] - bottomRightPos[1] + 1;
    map.cols = (upperLeftTile.data.cols - 2) * numberOfTilesX;
    map.rows = (upperLeftTile.data.rows - 2) * numberOfTilesY;
    map.data.resize(map.cols * map.rows);
    for (Int tileY = upperLeftPos[1]; tileY >= bottomRightPos[1]; --tileY)
    {
        for (Int tileX = upperLeftPos[0]; tileX <= bottomRightPos[0]; ++tileX)
        {
            const auto& tile = puzzle(tileX, tileY);
            const auto& mapUpperLeftX = (tileX - upperLeftPos[0]) * (tile.data.cols - 2);
            const auto& mapUpperLeftY = (upperLeftPos[1] - tileY) * (tile.data.rows - 2);
            for (Int y = 0; y < tile.data.rows - 2; ++y)
            {
                for (Int x = 0; x < tile.data.cols - 2; ++x)
                {
                    map(mapUpperLeftX + x, mapUpperLeftY + y) = tile.data(x + 1, y + 1);
                }
            }
        }
    }
    return map;
}

Grid2D<Tile> solvePuzzle(const std::vector<Tile>& p_tiles)
{
    std::vector<Tile> tiles{p_tiles};
    Grid2D<Tile> puzzle;
    puzzle(0, 0) = tiles.back();
    tiles.pop_back();
    std::queue<Vec2D> positions;
    positions.push({0, 0});
    while (!positions.empty())
    {
        auto& currentPosition = positions.front();
        positions.pop();
        Tile& currentTile = puzzle(currentPosition[0], currentPosition[1]);
        const auto freeNeighbors = puzzle.getNotExistingNeighborsPositions(currentPosition[0], currentPosition[1]);
        for (const auto& position : freeNeighbors)
        {
            bool success = tryToPlaceTileAtPosition(position, puzzle, tiles);
            if (success)
            {
                positions.push(position);
            }
        }
    }
    return puzzle;
}

Int part1(const std::vector<Tile>& p_tiles)
{
    const auto puzzle = solvePuzzle(p_tiles);
    const auto map = convertGrid2DToArray2DWithGapsAndBordersRemoved(puzzle);

//    std::cout << std::endl << map << std::endl;

    Vec2D upperLeftPos = findUpperLeftCorner(puzzle);
    Vec2D bottomRightPos = findBottomRightCorner(puzzle);
    Int result = 1;
    result *= puzzle(upperLeftPos[0], upperLeftPos[1]).id; // upper left
    result *= puzzle(bottomRightPos[0], upperLeftPos[1]).id; // upper right
    result *= puzzle(bottomRightPos[0], bottomRightPos[1]).id; // bottom right
    result *= puzzle(upperLeftPos[0], bottomRightPos[1]).id; // bottom left
    return result;
}

bool doesPatternMatch(const Array2D<char>& map, const Array2D<char>& pattern, Vec2D upperLeftPos)
{
    Vec2D bottomRightPos = {upperLeftPos[0] + pattern.cols - 1, upperLeftPos[1] + pattern.rows - 1};
    if (!map.isValidIndex(upperLeftPos[0], upperLeftPos[1]) ||
        !map.isValidIndex(bottomRightPos[0], bottomRightPos[1]))
        return false;
    for (Int y = upperLeftPos[1]; y <= bottomRightPos[1]; ++y)
    {
        for (Int x = upperLeftPos[0]; x <= bottomRightPos[0]; ++x)
        {
            if (pattern(x - upperLeftPos[0], y - upperLeftPos[1]) == '#' && map(x,y) == '.')
            {
                return false;
            }
        }
    }
//    std::cerr << "Found sea monster at: " << upperLeftPos[0] << ", " << upperLeftPos[1] << std::endl;
    return true;
}

void replacePattern(Array2D<char>& map, const Array2D<char>& pattern, Vec2D upperLeftPos)
{
    // assume the pattern matches
    Vec2D bottomRightPos = {upperLeftPos[0] + pattern.cols - 1, upperLeftPos[1] + pattern.rows - 1 };
    for (Int y = upperLeftPos[1]; y <= bottomRightPos[1]; ++y)
    {
        for (Int x = upperLeftPos[0]; x <= bottomRightPos[0]; ++x)
        {
            if (pattern(x - upperLeftPos[0], y - upperLeftPos[1]) == '#')
            {
                map(x,y) = 'O';
            }
        }
    }
}

bool replacePatterns(Array2D<char>& map, const Array2D<char>& pattern)
{
    bool matched = false;
    for (Int y = 0; y < map.rows; ++y)
    {
        for (Int x = 0; x < map.cols; ++x)
        {
            if (doesPatternMatch(map, pattern, Vec2D{x,y}))
            {
                replacePattern(map, pattern, Vec2D{x,y});
                matched = true;
            }
        }
    }
    return matched;
}

Int part2(const std::vector<Tile>& p_tiles)
{
    const auto puzzle = solvePuzzle(p_tiles);
    auto map = convertGrid2DToArray2DWithGapsAndBordersRemoved(puzzle);

    std::string patternStr = "                  # "
                             "#    ##    ##    ###"
                             " #  #  #  #  #  #   ";
    Array2D<char> pattern{};
    pattern.data = std::vector<char>(patternStr.begin(), patternStr.end());
    pattern.rows = 3;
    pattern.cols = 20;
//    std::cout << pattern << std::endl;

    Int count = 0;
    for (auto mapVariant : getAllVariantsOfMap(map))
    {
        if (replacePatterns(mapVariant, pattern))
        {
//            std::cout << mapVariant <<std::endl;
            count += std::count(mapVariant.data.begin(), mapVariant.data.end(), '#');
            break;
        }
    }
    return count;
}

int main()
{
    const auto& tiles = loadTiles("day20.txt");
    std::cout << "part 1: " << part1(tiles) << std::endl;
    std::cout << "part 2: " << part2(tiles) << std::endl;
}
