#include <random>
#include <ctime>
#include <algorithm>
#include "HiUtils.h"
#include "uuid4.h"
#include <fstream>
#include <regex>
int HiUtils::random_int(int minInt, int maxInt) {
    std::random_device                                       dev;
    std::mt19937                                             rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(minInt, maxInt);
    return dist6(rng);
}
std::string HiUtils::uuid() {
    char uuidBuff[UUID4_LEN]{0};
    uuid4_generate(uuidBuff);
    return uuidBuff;
}
std::vector<std::string> HiUtils::split_str(const std::string &strv, char delim) {
    std::vector<std::string> sv;
    std::istringstream       iss(strv);
    std::string              temp;
    while (std::getline(iss,
                        temp,
                        delim)) {
        sv.emplace_back(std::move(temp));
    }
    return sv;
}
std::vector<std::string_view> HiUtils::split_str(std::string_view strv, std::string_view delims) {
    std::vector<std::string_view> output;
    size_t                        first = 0;
    while (first < strv.size()) {
        const auto second = strv.find_first_of(delims,
                                               first);
        if (first != second) {
            output.emplace_back(strv.substr(first,
                                            second - first));
        }
        if (second == std::string_view::npos) {
            break;
        }
        first = second + 1;
    }
    return output;
}
long HiUtils::now_seconds() {
    auto now      = std::chrono::system_clock::now();
    auto epoch    = now.time_since_epoch();
    auto value    = std::chrono::duration_cast<std::chrono::seconds>(epoch);
    long duration = value.count();
    return duration;
}
std::string HiUtils::replace_all(std::string str, const std::string &from, const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}
char HiUtils::toLowerCh(char c) {
    return static_cast<char>( std::tolower(c));
}

void HiUtils::toLowerInPlace(std::string &s) {
    std::transform(s.begin(), s.end(), s.begin(), toLowerCh);
}

std::string HiUtils::toLower(std::string const &s) {
    std::string lc = s;
    toLowerInPlace(lc);
    return lc;
}

std::vector<std::string> HiUtils::splitMatch(const std::string& matchName, const std::string& matchRegex)
{
	std::vector<std::string> result;
	std::smatch sMatch;
	bool retStat = std::regex_match(matchName, sMatch, std::regex(matchRegex));
	if (retStat)
	{
		for (int i = 0; i < sMatch.size(); i++)
			result.push_back(sMatch[i]);
	}
	return result;
}