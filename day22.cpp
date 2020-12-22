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

// Stolen from boost (https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x)
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::deque<T>& deque)
{
    os << "{";
    auto it = deque.cbegin();
    for (; it != deque.cend() && it != deque.cend() - 1; ++it)
    {
        os << *it << ", ";
    }
    if (it != deque.cend())
    {
        os << *it;
    }
    os << "}";
    return os;
}

struct Game
{
    std::deque<Int> player1Deck;
    std::deque<Int> player2Deck;

    friend bool operator==(const Game& game1, const Game& game2)
    {
        return game1.player1Deck == game2.player1Deck && game1.player2Deck == game2.player2Deck;
    }

    friend bool operator<(const Game& game1, const Game& game2)
    {
        return game1.player1Deck < game2.player1Deck ||
            (game1.player1Deck == game2.player1Deck && game1.player2Deck < game2.player2Deck);
    }
};

template<typename T>
struct DequeHasher
{
    std::size_t operator()(const std::deque<T>& deque) const
    {
        std::size_t seed = 0;
        for (const auto& el : deque)
            hash_combine(seed, el);
        return seed;
    }
};

struct GameHasher
{
    std::size_t operator()(const Game& game) const
    {
        std::size_t seed = 0;
        hash_combine(seed, dequeHasher(game.player1Deck));
        hash_combine(seed, dequeHasher(game.player2Deck));
        return seed;
    }

    DequeHasher<Int> dequeHasher;
};

std::ostream& operator<<(std::ostream& os, const Game& game)
{
    os << "{player1: " << game.player1Deck << ", player2: " << game.player2Deck << "}";
    return os;
}

Game loadGame(const std::string& filename)
{
    std::ifstream ifs{filename};
    Game game{};
    std::string line;
    std::getline(ifs, line); // player 1
    while (std::getline(ifs, line) && !line.empty())
    {
        std::istringstream iss{line};
        Int n;
        iss >> n;
        game.player1Deck.push_back(n);
    }
    std::getline(ifs, line); // player 2
    while (std::getline(ifs, line) && !line.empty())
    {
        std::istringstream iss{line};
        Int n;
        iss >> n;
        game.player2Deck.push_back(n);
    }
    return game;
}

Game playGame(const Game& initialGame)
{
    auto game = initialGame;
    while (!game.player1Deck.empty() && !game.player2Deck.empty())
    {
        Int player1TopCard = game.player1Deck.front();
        game.player1Deck.pop_front();
        Int player2TopCard = game.player2Deck.front();
        game.player2Deck.pop_front();
        if (player1TopCard > player2TopCard)
        {
            game.player1Deck.push_back(player1TopCard);
            game.player1Deck.push_back(player2TopCard);
        }
        else
        {
            game.player2Deck.push_back(player2TopCard);
            game.player2Deck.push_back(player1TopCard);
        }
    }
    return game;
}

Int calculateScore(const Game& finalGame)
{
    const auto& winnersDeck = finalGame.player1Deck.empty() ? finalGame.player2Deck : finalGame.player1Deck;
    Int score = 0;
    for (Int i = 0; i < winnersDeck.size(); ++i)
        score += (i + 1) * winnersDeck[winnersDeck.size() - 1 - i];
    return score;
}

struct RecursiveGameResult
{
    Game game;
    Int winner{};
};

RecursiveGameResult playRecursiveGame(const Game& initialGame)
{
    auto game = initialGame;
    std::unordered_set<Game, GameHasher> previousRounds;
//    std::set<Game> previousRounds;
    while (!game.player1Deck.empty() && !game.player2Deck.empty())
    {
        if (previousRounds.count(game) > 0)
            return {game, 1};
        previousRounds.insert(game);

        Int player1TopCard = game.player1Deck.front();
        game.player1Deck.pop_front();
        Int player2TopCard = game.player2Deck.front();
        game.player2Deck.pop_front();

        Int winner;
        if (player1TopCard <= game.player1Deck.size() && player2TopCard <= game.player2Deck.size())
        {
            Game recursiveGame{};
            recursiveGame.player1Deck.insert(recursiveGame.player1Deck.end(),
                                             game.player1Deck.begin(), game.player1Deck.begin() + player1TopCard);
            recursiveGame.player2Deck.insert(recursiveGame.player2Deck.end(),
                                             game.player2Deck.begin(), game.player2Deck.begin() + player2TopCard);
            winner = playRecursiveGame(recursiveGame).winner;
        }
        else
        {
            winner = player1TopCard > player2TopCard ? 1 : 2;
        }

        if (winner == 1)
        {
            game.player1Deck.push_back(player1TopCard);
            game.player1Deck.push_back(player2TopCard);
        }
        else
        {
            game.player2Deck.push_back(player2TopCard);
            game.player2Deck.push_back(player1TopCard);
        }
    }
    return {game, game.player1Deck.empty() ? 2 : 1};
}

int main()
{
    const auto game = loadGame("day22.txt");
    std::cout << "part 1: " << calculateScore(playGame(game)) << std::endl;
    auto start = std::chrono::steady_clock::now();
    std::cout << "part 2: " << calculateScore(playRecursiveGame(game).game) << std::endl;
    auto end = std::chrono::steady_clock::now();
    std::cout << "Part 2 took "
         << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1'000'000'000.0
         << " s" << std::endl;
}
