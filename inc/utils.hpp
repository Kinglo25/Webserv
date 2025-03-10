#pragma once

#include <string>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>

template<typename T>
std::string	to_string(T val)
{
	std::stringstream ss;

	ss << val;
	return (ss.str());
}

bool isDir(std::string path);
std::string urlDecode(const std::string& url);
unsigned long	strToUl(std::string nbr, bool &check);
