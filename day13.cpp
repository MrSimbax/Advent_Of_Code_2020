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
#include <boost/algorithm/string.hpp>

using Int = long long int;

struct Input
{
    Int departureTime;
    std::vector<Int> busIds;
    std::vector<Int> busIdsWithZeros;
};

Input loadInput(const std::string& filename)
{
    std::ifstream ifs{filename};
    Input input{};
    ifs >> input.departureTime;
    std::string line;
    std::getline(ifs, line); // empty line
    std::getline(ifs, line);
    std::vector<std::string> splitLine;
    boost::split(splitLine, line, boost::is_any_of(","));
    for (const auto& busId : splitLine)
    {
        if (busId == "x")
            input.busIdsWithZeros.push_back(0);
        else
            input.busIdsWithZeros.push_back(std::stoll(busId));

        if (busId != "x")
            input.busIds.push_back(std::stoll(busId));
    }
    return input;
}

Int part1(const Input& input)
{
    // let t >= 0
    // a bus with id = id_i leaves at time departureTime + t iff
    // departureTime + t = 0 (mod id_i)
    for (Int t = 0; ; ++t)
    {
        for (const auto busId : input.busIds)
        {
            Int m = input.departureTime % busId;
            m += t % busId;
            m %= busId;
            if (m == 0)
            {
                return t * busId;
            }
        }
    }
}

struct XGCDResult
{
    Int gcd;
    Int s;
    Int t;
};

// gcd(a,b) = s * a + t * b
XGCDResult xGCD(Int a, Int b)
{
    bool swp = false;
    if (a < b)
    {
        swp = true;
        std::swap(a, b);
    }
    if (a == b || b == 0) return XGCDResult{a, 1, 0};

    // a = 1 * a + 0 * b
    Int old_r = a;
    Int old_s = 1;
    Int old_t = 0;

    // b = 0 * a + 1 * b
    Int r = b;
    Int s = 0;
    Int t = 1;

    // consider the system of equations:
    // (1) old_r = old_s * a + old_t * b
    // (2) r = s * a + t * b
    // we want r = GCD(a,b) = GCD(old_r, r) = GCD(r, old_r mod r)
    // so
    // new_r = old_r mod r = old_r - floor(old_r / r) * r
    // let q = floor(old_r / r)
    // subtract equation (2) * (-q) from equation (1)
    // so we get
    // (1) new_r = (old_s - q * s) * a + (old_t - q * t) * b
    // (2) r = s * a + t * b
    // equation (2) becomes old
    // repeat until r = 0 (the stopping condition for Euclidean algorithm)
    // by then old_r = GCD(a,b) and old_t and old_s are as desired
    while (r != 0)
    {
        Int tmp_r = r;
        Int tmp_s = s;
        Int tmp_t = t;

        Int q = old_r / r;

        r = old_r - q * r;
        s = old_s - q * s;
        t = old_t - q * t;

        old_r = tmp_r;
        old_s = tmp_s;
        old_t = tmp_t;
    }

    if (swp) std::swap(old_s, old_t);
    return XGCDResult{old_r, old_s, old_t};
}

struct BadInput : public std::exception {};

// x^-1 mod n
Int modInv(Int x, Int n)
{
    auto xGCDResult = xGCD(x, n);
    if (xGCDResult.gcd != 1)
        throw BadInput{};
    Int ret = xGCDResult.s;
    if (ret < 0)
        ret += n;
    return ret;
}

// Find x such that
// x = y_1 mod n_1
// x = y_2 mod n_2
// ...
// x = y_k mod n_k
// and 0 <= x < N = n_1 * n_2 * ... * n_k
// where
// 0 <= y_i < n_i for all i
// n_i and n_j for all i != j are co-prime
Int CRT(const std::vector<Int>& y, const std::vector<Int>& n)
{
    // proof:
    // (uniqueness)
    // let x,y be solutions
    // then x-y divides n_i
    // therefore x-y divides N
    // so x-y=0 (mod N) <-> x=y (mod N)
    // (existence by construction)
    // let N_i = N / n_i -- product of all n_j except n_i
    // since n_i are co-prime, N_i and n_i are co-prime
    // so there is a number 0 <= M_i < n_i such that M_i * N_i = 1 (mod n_i)
    // we'll show that x = sum(y_i * M_i * N_i) is a solution:
    // x = y_i * M_i * N_i (mod n_i) // all elements of the sum are a multiple of n_i except maybe the i-th element
    // x = y_i * 1 (mod n_i)
    // x = y_i (mod n_i)
    // QED
    Int N = std::accumulate(n.begin(), n.end(), static_cast<Int>(1), std::multiplies<Int>{});
    Int x = 0;
    for (Int i = 0; i < n.size(); ++i)
    {
        Int q = N / n[i];
        x += (y[i] * q * modInv(q, n[i])) % N;
        x %= N;
    }
    return x;
}

Int part2(const Input& input)
{
    // find t such that
    // i-th bus leaves at t time
    // i+1-ith bus leaves at t+1 time
    // and so on
    // This is equivalent to solving the system of equations:
    // t = 0 (mod id_1)           t = 0 (mod id_1)
    // t + 1 = 0 (mod id_2)       t = -1 (mod id_2)
    // ...                    <-> ...
    // t + i = 0 (mod id_i)       t = -i (mod id_i)
    // ...                        ...
    // we skip all i for which there's no bus
    // such system of equations can be solved using the Chinese Remainder Theorem
    std::vector<Int> y;
    y.reserve(input.busIds.size());
    for (Int i = 0; i < input.busIdsWithZeros.size(); ++i)
    {
        const auto busId = input.busIdsWithZeros[i];
        if (busId > 0)
        {
            Int yi = busId - i;
            while (yi < 0)
                yi += busId;
            yi %= busId;
            y.push_back(yi);
        }
    }
    return CRT(y, input.busIds);
}

int main()
{
    const auto& input = loadInput("day13.txt");
    std::cout << "part 1: " << part1(input) << std::endl;
    std::cout << "part 2: " << part2(input) << std::endl;
}
