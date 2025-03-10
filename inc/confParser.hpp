#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "utils.hpp"
#include "Server.hpp"
#include "statusCode.hpp"

int	isrule(int c);
int	isfile(int c);
int	isroute(int c);
int	islink(int c);
int	isother(int c);
std::string	addLine(int line, std::string message);
std::string	getNextElem(std::string& line, int& i, int (*f)(int));
void	closeAndThrow(std::string message, int line, std::ifstream& file);
void	listenCheck(std::string& line, int& i, int&j, Server& server, std::ifstream& ifile);
void	locationCheck(std::string& line, int&i, int& j, Server& server, std::ifstream& ifile);
void	checkEndOfLine(std::string& line, int& i, int j, bool semicolon, std::ifstream& file);
void	bodySizeCheck(std::string line, int& i, int& j, Server& server, std::ifstream& ifile);
void	errorPageCheck(std::string& line, int&i, int& j, Server& server, std::ifstream& ifile);
void	serverNameCheck(std::string& line, int& i, int& j, Server& server, std::ifstream& ifile);
void	serverRootCheck(std::string& line, int& i, int& j, Server& server, std::ifstream& ifile);
void	serverCheck(bool& server, std::vector<Server>& servers, std::string& line, int& i, int& j, std::ifstream& ifile);
void	endOfServerCheck(std::string& line, int&i, int& j, int& serverIdx, Server server, std::vector<u_int32_t>& ports, std::vector<std::string>& names, std::string& elem, bool& bserver, std::ifstream& ifile);
