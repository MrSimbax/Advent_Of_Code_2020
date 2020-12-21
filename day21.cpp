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

template<class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& set)
{
    os << "{";
    Int size = set.size();
    auto it = set.cbegin();
    for (; it != set.cend() && size > 1; ++it, --size)
    {
        os << *it << ", ";
    }
    if (it != set.cend())
    {
        os << *it;
    }
    os << "}";
    return os;
}

template<class K, class V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& map)
{
    os << "{";
    Int size = map.size();
    auto it = map.cbegin();
    for (; it != map.cend() && size > 1; ++it, --size)
    {
        os << it->first << " -> " << it->second << ", ";
    }
    if (it != map.cend())
    {
        os << it->first << " -> " << it->second;
    }
    os << "}";
    return os;
}

struct Food
{
    std::set<std::string> ingredients;
    std::set<std::string> allergens;
};

std::ostream& operator<<(std::ostream& os, const Food& food)
{
    os << "{ingredients: " << food.ingredients << ", allergens: " << food.allergens << "}";
    return os;
}

using FoodList = std::vector<Food>;

Food loadFood(const std::string& line)
{
    std::istringstream iss{line};
    std::string ingredient;
    Food food{};
    std::set<std::string> ingredientNames;
    std::set<std::string> allergensNames;
    while (iss >> ingredient && ingredient != "(contains")
        food.ingredients.insert(ingredient);
    std::string allergen;
    while (iss >> allergen)
        food.allergens.insert(allergen.substr(0, allergen.size() - 1));
    return food;
}

FoodList loadFoodList(const std::string& filename)
{
    std::ifstream ifs{filename};
    FoodList foodList{};
    std::string line;
    while (std::getline(ifs, line))
        foodList.push_back(loadFood(line));
    return foodList;
}

std::map<std::string, std::set<std::string>> findSetOfPossibleIngredientsForEachAllergen(const FoodList& foodList)
{
    std::map<std::string, std::set<std::string>> possibleIngredientsForAllergens;
    for (const auto& food : foodList)
    {
        for (const auto& allergen : food.allergens)
        {
            if (possibleIngredientsForAllergens.count(allergen) == 0)
                possibleIngredientsForAllergens[allergen] = food.ingredients;
            else
            {
                std::set<std::string>& currentSet = possibleIngredientsForAllergens[allergen];
                std::set<std::string> intersection;
                std::set_intersection(
                        food.ingredients.begin(), food.ingredients.end(),
                        currentSet.begin(), currentSet.end(),
                        std::inserter(intersection, intersection.begin()));
                possibleIngredientsForAllergens[allergen] = std::move(intersection);
            }
        }
    }
    return possibleIngredientsForAllergens;
}

std::set<std::string> findSetOfAllIngredients(const FoodList& foodList)
{
    std::set<std::string> allIngredients;
    for (const auto& food : foodList)
    {
            std::set<std::string> setUnion;
            std::set_union(
                    food.ingredients.begin(), food.ingredients.end(),
                    allIngredients.begin(), allIngredients.end(),
                    std::inserter(setUnion, setUnion.begin()));
            allIngredients = std::move(setUnion);
    }
    return allIngredients;
}

std::set<std::string> findSetOfAllergens(const FoodList& foodList)
{
    std::set<std::string> allAllergens;
    for (const auto& food : foodList)
    {
        std::set<std::string> setUnion;
        std::set_union(
                food.allergens.begin(), food.allergens.end(),
                allAllergens.begin(), allAllergens.end(),
                std::inserter(setUnion, setUnion.begin()));
        allAllergens = std::move(setUnion);
    }
    return allAllergens;
}

std::set<std::string> findSetOfDangerousIngredients(const FoodList& foodList)
{
    auto possibleIngredientsForAllergens = findSetOfPossibleIngredientsForEachAllergen(foodList);
    std::set<std::string> dangerousIngredients;
    for (const auto& [allergen, possibleIngredients] : possibleIngredientsForAllergens)
    {
        std::set<std::string> setUnion;
        std::set_union(
                possibleIngredients.begin(), possibleIngredients.end(),
                dangerousIngredients.begin(), dangerousIngredients.end(),
                std::inserter(setUnion, setUnion.begin()));
        dangerousIngredients = std::move(setUnion);
    }
    return dangerousIngredients;
}

std::set<std::string> findSetOfSafeIngredients(const FoodList& foodList)
{
    auto allIngredients = findSetOfAllIngredients(foodList);
    auto dangerousIngredients = findSetOfDangerousIngredients(foodList);
    std::set<std::string> setDifference;
    std::set_difference(
            allIngredients.begin(), allIngredients.end(),
            dangerousIngredients.begin(), dangerousIngredients.end(),
            std::inserter(setDifference, setDifference.begin()));
    return setDifference;
}

Int part1(const FoodList& foodList)
{
    auto safeIngredients = findSetOfSafeIngredients(foodList);
    Int count = 0;
    for (const auto& food : foodList)
        for (const auto& safeIngredient : safeIngredients)
            count += food.ingredients.count(safeIngredient);
    return count;
}

std::map<std::string, std::string> findAllergensToIngredientsMap(const FoodList& foodList)
{
    auto possibleIngredientsForAllergens = findSetOfPossibleIngredientsForEachAllergen(foodList);
    std::map<std::string, std::string> allergensToIngredientsMap;
    auto allergensRemaining = findSetOfAllergens(foodList);
    while (!allergensRemaining.empty())
    {
        auto singleton = std::find_if(possibleIngredientsForAllergens.begin(), possibleIngredientsForAllergens.end(),
                                      [](const decltype(possibleIngredientsForAllergens)::value_type& pair)
                                      {
                                            return pair.second.size() == 1;
                                      });
        auto allergen = singleton->first;
        auto ingredient = *(singleton->second.begin());
        for (auto& set : possibleIngredientsForAllergens)
            set.second.erase(ingredient);
        allergensToIngredientsMap[allergen] = ingredient;
        allergensRemaining.erase(allergen);
    }
    return allergensToIngredientsMap;
}

std::string part2(const FoodList& foodList)
{
    auto allergensToIngredientsMap = findAllergensToIngredientsMap(foodList);
    // print it in a copyable way
    std::ostringstream oss;
    Int size = allergensToIngredientsMap.size();
    auto it = allergensToIngredientsMap.cbegin();
    for (; it != allergensToIngredientsMap.cend() && size > 1; ++it, --size)
        oss << it->second << ",";
    if (it != allergensToIngredientsMap.cend())
        oss << it->second;
    return oss.str();
}

int main()
{
    const auto& foodList = loadFoodList("day21.txt");
    std::cout << "part 1: " << part1(foodList) << std::endl;
    std::cout << "part 2: " << part2(foodList) << std::endl;
}
