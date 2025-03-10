#include "utils.hpp"

unsigned long	strToUl(std::string nbr, bool &check)
{
	char *end;
	unsigned long	val = std::strtoul(nbr.c_str(), &end, 10);
	if (*end != '\0')
		check = false;
	return (val);
}

std::string urlDecode(const std::string& url)
{
    std::string decoded;
    for (std::size_t i = 0; i < url.length(); ++i) {
        if (url[i] == '%') {
            if (i + 2 < url.length() && std::isxdigit(url[i + 1]) && std::isxdigit(url[i + 2])) {
                std::istringstream hexStream(url.substr(i + 1, 2));
                int hexValue;
                hexStream >> std::hex >> hexValue;
                decoded += static_cast<char>(hexValue);
                i += 2;
            } else {
                decoded += '%';
            }
        } else if (url[i] == '+') {
            decoded += ' ';
        } else {
            decoded += url[i];
        }
    }
    return decoded;
}

bool	isDir(std::string path)
{
	struct stat statbuff;
	if (stat(path.c_str(), &statbuff) || !S_ISDIR(statbuff.st_mode))
		return (false);
	return (true);
}
