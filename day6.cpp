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

using GroupForms = std::vector<std::string>;

struct GroupAnswers
{
    std::array<Int, 26> answersCount;
    Int groupSize;
};

GroupForms loadGroupForms(const std::string& groupFormsStr)
{
    std::istringstream iss{groupFormsStr};
    std::string line;
    GroupForms groupForms;
    while (std::getline(iss, line))
    {
        groupForms.push_back(line);
    }
    return groupForms;
}

std::vector<GroupForms> loadAllGroupForms(const std::string& inputFilename)
{
    std::ifstream ifs{inputFilename};
    std::vector<GroupForms> allGroupForms;
    std::string groupFormsStr{};
    std::string line;
    while (std::getline(ifs, line))
    {
        if (line.empty())
        {
            allGroupForms.push_back(loadGroupForms(groupFormsStr));
            groupFormsStr = "";
        }
        else
        {
            groupFormsStr += line + "\n";
        }
    }
    if (!groupFormsStr.empty())
    {
        allGroupForms.push_back(loadGroupForms(groupFormsStr));
    }
    return allGroupForms;
}

GroupAnswers getGroupAnswers(const GroupForms& forms)
{
    GroupAnswers answers{};
    for (const auto& personForm : forms)
    {
        for (const auto question : personForm)
        {
            ++answers.answersCount[question - 'a'];
        }
    }
    answers.groupSize = forms.size();
    return answers;
}

Int countQuestionsAnsweredByAnyPerson(const GroupAnswers& answers)
{
    return std::count_if(answers.answersCount.begin(), answers.answersCount.end(),
                         [](Int count) { return count > 0; });
}

Int countQuestionsAnsweredByAllPeople(const GroupAnswers& answers)
{
    return std::count_if(answers.answersCount.begin(), answers.answersCount.end(),
                         [&answers](Int count) { return count == answers.groupSize; });
}

template<typename Predicate>
Int countAllQuestionsIf(const std::vector<GroupForms>& allGroupForms, Predicate predicate)
{
    return std::accumulate(allGroupForms.begin(), allGroupForms.end(), static_cast<Int>(0),
                    [&predicate](Int a, const GroupForms& groupForms)
                    {
                        return a + predicate(getGroupAnswers(groupForms));
                    });
}

int main()
{
    std::cout << "part 1: "
        << countAllQuestionsIf(loadAllGroupForms("day6.txt"), countQuestionsAnsweredByAnyPerson)
        << std::endl;
    std::cout << "part 2: "
        << countAllQuestionsIf(loadAllGroupForms("day6.txt"), countQuestionsAnsweredByAllPeople)
        << std::endl;
}
