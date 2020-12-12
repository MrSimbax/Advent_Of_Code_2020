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

struct Edge
{
    std::string color;
    Int count;
};

struct Vertex
{
    std::string color;
    std::vector<Edge> edges;
};

std::string parseColor(std::istream& iss)
{
    std::string color1;
    std::string color2;
    iss >> color1 >> color2;
    return color1 + " " + color2;
}

Edge parseEdge(std::istream& iss)
{
    Edge edge{};
    if (iss.eof()) return edge;
    std::string num;
    iss >> num;
    if (num == "no")
    {
        std::string skip;
        iss >> skip >> skip; // "no other bags."
        return edge;
    }
    edge.count = std::stoll(num);
    edge.color = parseColor(iss);
    std::string skip;
    iss >> skip; // "bag," or "bag." or "bags," or "bags."
    return edge;
}

Vertex parseVertex(const std::string& rule)
{
    std::istringstream iss(rule);
    Vertex container{};
    container.color = parseColor(iss);
    std::string skip;
    iss >> skip >> skip; // "bags contain"
    auto edge = parseEdge(iss);
    while (!edge.color.empty())
    {
        container.edges.push_back(std::move(edge));
        edge = parseEdge(iss);
    }
    return container;
}

std::unordered_map<std::string, Vertex> loadContainers(const std::string& filename)
{
    std::ifstream ifs{filename};
    std::string line;
    std::unordered_map<std::string, Vertex> containers;
    while (std::getline(ifs, line))
    {
        auto container = parseVertex(line);
        containers[container.color] = std::move(container);
    }
    return containers;
}

std::unordered_map<std::string, Vertex> transposed(const std::unordered_map<std::string, Vertex>& graph)
{
    std::unordered_map<std::string, Vertex> transposedGraph;
    for (const auto& [color, vertex] : graph)
    {
        transposedGraph[color] = Vertex{color,{}};
    }
    for (const auto& [color, vertex] : graph)
    {
        for (const auto& edge : vertex.edges)
        {
            transposedGraph[edge.color].edges.push_back(Edge{color, edge.count});
        }
    }
    return transposedGraph;
}

Int part1(const std::unordered_map<std::string, Vertex>& graph)
{
    const auto transposedGraph = transposed(graph);
    std::unordered_set<std::string> visitedVertices{};
    std::queue<std::string> queue{};
    visitedVertices.insert("shiny gold");
    queue.push("shiny gold");
    Int count = 0;
    while (!queue.empty())
    {
        auto color = queue.front();
        queue.pop();
        const auto& vertex = transposedGraph.at(color);
        for (const auto& edge : vertex.edges)
        {
            if (visitedVertices.count(edge.color) == 0)
            {
                ++count;
                visitedVertices.insert(edge.color);
                queue.push(edge.color);
            }
        }
    }
    return count;
}

Int part2(const std::unordered_map<std::string, Vertex>& graph, const std::string& root)
{
    Int count = 1;
    const auto& vertex = graph.at(root);
    for (const auto& edge : vertex.edges)
    {
        count += edge.count * part2(graph, edge.color);
    }
    return count;
}

int main()
{
    std::cout << "part 1: " << part1(loadContainers("day7.txt")) << std::endl;
    std::cout << "part 2: " << part2(loadContainers("day7.txt"), "shiny gold") - 1 << std::endl;
}
