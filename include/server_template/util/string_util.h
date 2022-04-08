#ifndef SERVER_TEMPLATE_UTIL_STRING_UTIL_H_
#define SERVER_TEMPLATE_UTIL_STRING_UTIL_H_

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include "util_ns.h"

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

class StringUtil
{
public:
	static std::vector<std::string> split(const std::string &str, char seperator, bool ignoreEmptyString = false)
	{
		std::vector<std::string> resList;
		std::string::size_type pos1, pos2;
		pos1 = 0;
		pos2 = str.find(seperator);
		while (std::string::npos != pos2)
		{
			if (!ignoreEmptyString || (pos1 != pos2))
			{
				resList.push_back(str.substr(pos1, pos2 - pos1));
			}
			pos1 = pos2 + 1;
			pos2 = str.find(seperator, pos1);
		}
		if (pos1 != str.length())
		{
			resList.push_back(str.substr(pos1));
		}

		return resList;
	}

	static std::vector<std::string> split(const std::string &str, const char *seperators, bool ignoreEmptyString = false)
	{
		std::vector<std::string> resList;
		std::string::size_type pos1, pos2;
		pos1 = 0;
		pos2 = str.find_first_of(seperators);
		while (std::string::npos != pos2)
		{
			if (!ignoreEmptyString || (pos1 != pos2))
			{
				resList.push_back(str.substr(pos1, pos2 - pos1));
			}
			pos1 = pos2 + 1;
			pos2 = str.find_first_of(seperators, pos1);
		}
		if (pos1 != str.length())
		{
			resList.push_back(str.substr(pos1));
		}

		return resList;
	}

	static std::string trim(std::string str, const char *exlude = "\t\n\r ")
	{
		if (!str.empty())
		{
			str.erase(0, str.find_first_not_of(exlude));
			str.erase(str.find_last_not_of(exlude) + 1);
		}
		return str;
	}

	static void batchTrim(std::vector<std::string> &array, const char *exlude = "\t\n\r ")
	{
		for (int i = 0; i < (int)array.size(); i++)
		{
			array[i] = trim(array[i], exlude);
		}
	}

	template <typename... Args>
	static std::string format(const std::string &format, Args... args)
	{
		int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
		if (size_s <= 0)
		{
			throw std::runtime_error("Error during formatting.");
		}
		auto size = static_cast<size_t>(size_s);
		std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, format.c_str(), args...);
		return std::string(buf.get(), buf.get() + size - 1);
	}

	static void toLower(const std::string &src, std::string &dest)
	{
		dest.resize(src.size());
		std::transform(src.begin(), src.end(), dest.begin(), ::tolower);
	}

	static void toUpper(const std::string &src, std::string &dest)
	{
		dest.resize(src.size());
		std::transform(src.begin(), src.end(), dest.begin(), ::toupper);
	}

	static bool isUpper(char input)
    {
        return input >= 'A' && input <= 'Z';
    }

    static bool isLower(char input)
    {
        return input >= 'a' && input <= 'z';
    }

	static bool isAlpha(char input)
	{
		return isUpper(input) || isLower(input);
	}

	static bool isNumber(char input)
	{
		return input >= '0' && input <= '9';
	}

    static bool isLWSP(char input)
    {
        return input == ' ' || input == '\t';
    }

    static bool isCtls(char input)
    {
        return (input >= 0 && input <= 31) || input == 127;
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_STRING_UTIL_H_
