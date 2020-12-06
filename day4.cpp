#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>

#include <boost/algorithm/string.hpp>

using Int = long long int;

struct Passport
{
    std::string byr{}; // birth year
    std::string iyr{}; // issue year
    std::string eyr{}; // expiration year
    std::string hgt{}; // height
    std::string hcl{}; // hair color
    std::string ecl{}; // eye color
    std::string pid{}; // passport id
    std::string cid{}; // country id
};

Passport loadPassport(const std::string& passportStr)
{
    std::istringstream iss{passportStr};
    std::string fieldStr;
    Passport passport;
    while (iss >> fieldStr)
    {
        std::vector<std::string> splitStr;
        boost::split(splitStr, fieldStr, boost::is_any_of(":"));
        std::string key = splitStr[0];
        std::string value = splitStr[1];
        if (key == "byr")
            passport.byr = value;
        else if (key == "iyr")
            passport.iyr = value;
        else if (key == "eyr")
            passport.eyr = value;
        else if (key == "hgt")
            passport.hgt = value;
        else if (key == "hcl")
            passport.hcl = value;
        else if (key == "ecl")
            passport.ecl = value;
        else if (key == "pid")
            passport.pid = value;
        else if (key == "cid")
            passport.cid = value;
    }
//    std::cout << "Passport:" << std::endl;
//    std::cout << "byr = " << passport.byr << std::endl;
//    std::cout << "iyr = " << passport.iyr << std::endl;
//    std::cout << "eyr = " << passport.eyr << std::endl;
//    std::cout << "hgt = " << passport.hgt << std::endl;
//    std::cout << "hcl = " << passport.hcl << std::endl;
//    std::cout << "ecl = " << passport.ecl << std::endl;
//    std::cout << "pid = " << passport.pid << std::endl;
//    std::cout << "cid = " << passport.cid << std::endl;
//    std::cout << std::endl;
    return passport;
}

std::vector<Passport> loadPassports(const std::string& inputFilename)
{
    std::ifstream ifs{inputFilename};
    std::vector<Passport> passports;
    std::string passportStr{};
    std::string line;
    while (std::getline(ifs, line))
    {
        if (line.empty())
        {
            passports.push_back(loadPassport(passportStr));
            passportStr = "";
        }
        else
        {
            passportStr += line + "\n";
        }
    }
    if (!passportStr.empty())
    {
        passports.push_back(loadPassport(passportStr));
    }
    return passports;
}

bool isPassportValidPart1(const Passport& passport)
{
    return
            !passport.byr.empty() &&
            !passport.iyr.empty() &&
            !passport.eyr.empty() &&
            !passport.hgt.empty() &&
            !passport.hcl.empty() &&
            !passport.ecl.empty() &&
            !passport.pid.empty();
}

bool isByrFieldValid(const std::string& byr)
{
    if (byr.empty()) return false;
    if (byr.size() != 4) return false;
    if (!std::all_of(byr.begin(), byr.end(), isdigit)) return false;
    auto n = std::stoll(byr);
    return 1920 <= n && n <= 2002;
}

bool isIyrFieldValid(const std::string& iyr)
{
    if (iyr.empty()) return false;
    if (iyr.size() != 4) return false;
    if (!std::all_of(iyr.begin(), iyr.end(), isdigit)) return false;
    auto n = std::stoll(iyr);
    return 2010 <= n && n <= 2020;
}

bool isEyrFieldValid(const std::string& eyr)
{
    if (eyr.empty()) return false;
    if (eyr.size() != 4) return false;
    if (!std::all_of(eyr.begin(), eyr.end(), isdigit)) return false;
    auto n = std::stoll(eyr);
    return 2020 <= n && n <= 2030;
}

bool isHgtFieldValid(const std::string& hgt)
{
    if (hgt.empty()) return false;
    Int n;
    std::string unit;
    std::string postfix;
    std::istringstream iss{hgt};
    iss >> n;
    iss >> unit;
    iss >> postfix;
    if (!postfix.empty()) return false;
    if (unit == "cm") return 150 <= n && n <= 193;
    else if (unit == "in") return 59 <= n && n <= 76;
    else return false;
}

bool isHclFieldValid(const std::string& hcl)
{
    if (hcl.empty()) return false;
    if (hcl.size() != 7) return false;
    if (hcl[0] != '#') return false;
    return std::all_of(hcl.begin() + 1, hcl.end(),
                       [](char c) { return '0' <= c && c <= '9' || ('a' <= c && c <= 'f'); });
}

bool isEclFieldValid(const std::string& ecl)
{
    if (ecl.empty()) return false;
    return
        ecl == "amb" ||
        ecl == "blu" ||
        ecl == "brn" ||
        ecl == "gry" ||
        ecl == "grn" ||
        ecl == "hzl" ||
        ecl == "oth";
}

bool isPidFieldValid(const std::string& pid)
{
    if (pid.empty()) return false;
    if (pid.size() != 9) return false;
    return std::all_of(pid.begin(), pid.end(), isdigit);
}

bool isCidFieldValid(const std::string& cid)
{
    return true;
}

bool isPassportValidPart2(const Passport& passport)
{
    return
            isByrFieldValid(passport.byr) &&
            isIyrFieldValid(passport.iyr) &&
            isEyrFieldValid(passport.eyr) &&
            isHgtFieldValid(passport.hgt) &&
            isHclFieldValid(passport.hcl) &&
            isEclFieldValid(passport.ecl) &&
            isPidFieldValid(passport.pid) &&
            isCidFieldValid(passport.cid);
}

template<typename Validator>
Int countValidPassports(const std::vector<Passport>& passport, Validator validator)
{
    return std::count_if(passport.begin(), passport.end(), validator);
}

int main()
{
    std::cout << "part 1: " << countValidPassports(loadPassports("day4.txt"), isPassportValidPart1) << std::endl;
    std::cout << "part 2: " << countValidPassports(loadPassports("day4.txt"), isPassportValidPart2) << std::endl;
}
