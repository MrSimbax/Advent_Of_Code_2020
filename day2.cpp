#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>

using Int = long long int;

struct Policy
{
    Int minOccurrences;
    Int maxOccurrences;
    char character;
};

struct Password
{
    Policy policy{};
    std::string password;
};

Password loadPassword(const std::string& line)
{
    std::istringstream iss{line};
    Password password;
    char separator;
//    std::cout << line << std::endl;
    iss >> password.policy.minOccurrences;
    iss >> separator;
    iss >> password.policy.maxOccurrences;
    iss >> password.policy.character;
    iss >> separator;
    iss >> password.password;
//    std::cout << password.password << std::endl;
//    std::cout << password.policy.minOccurrences << std::endl;
//    std::cout << password.policy.maxOccurrences << std::endl;
//    std::cout << password.policy.character << std::endl;
//    std::cout << std::endl;
    return password;
}

std::vector<Password> loadPasswords(const std::string& inputFilename)
{
    std::ifstream ifs{inputFilename};
    std::vector<Password> passwords;
    std::string line;
    while (std::getline(ifs, line))
    {
        passwords.push_back(loadPassword(line));
    }
    return passwords;
}

bool isPasswordValidPart1(const Password& password)
{
    auto characterCount = std::count(password.password.begin(), password.password.end(), password.policy.character);
    return password.policy.minOccurrences <= characterCount && characterCount <= password.policy.maxOccurrences;
}

bool isPasswordValidPart2(const Password& password)
{
    Int position1 = password.policy.minOccurrences - 1;
    Int position2 = password.policy.maxOccurrences - 1;
    return (password.password[position1] == password.policy.character) ^ (password.password[position2] == password.policy.character);
}

template<typename Validator>
Int countValidPasswords(const std::vector<Password>& passwords, Validator validator)
{
    return std::count_if(passwords.begin(), passwords.end(), validator);
}

int main()
{
    std::cout << "part 1: " << countValidPasswords(loadPasswords("day2.txt"), isPasswordValidPart1) << std::endl;
    std::cout << "part 2: " << countValidPasswords(loadPasswords("day2.txt"), isPasswordValidPart2) << std::endl;
}
