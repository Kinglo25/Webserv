#pragma once

#include <map>
#include <string>
#include <fstream>
#include <algorithm>
#include <exception>

#include "utils.hpp"
#include "Error.hpp"
#include "Location.hpp"
#include "statusCode.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#define	KILO 1024
#define MEGA 1048576
#define	MAX_BODY_SIZE 1073741824
#define GENERAL_ERROR_PAGE "./assets/error.html"

size_t	getLastSlash(std::string path);
void	addFileToBody(HttpResponse& res, std::string file);
void	addDefaultErrorPageToBody(HttpResponse& res, int code, std::string name);

std::string getFileType(std::string file);
std::string executeCgi(Location& loc, std::string serverRoot, char **env);

class Server
{
private:
    u_int32_t	_port;
    std::string	_name;
    std::string	_root;
	unsigned long	_bodySize;
    std::map<int, std::string>	_errPages;
    std::map<std::string, Location>	_locations;
public:
    Server();

    void    checkValues();

    u_int32_t	getPort();
	std::string	getRoot();
    std::string	getName();
    std::string	getErrorPage(int code);

    void    setPort(u_int32_t port);
    void    setName(std::string name);
    void    setRoot(std::string root);
	void	setBodySize(std::string body);
    void    addErrorPage(int code, std::string path);
    void    addLocation(std::string path, Location loc);

    void	addErrorPageToBody(HttpResponse& res, int code);

    Location&   findLocation(std::string& path);
	std::string	execute(HttpRequest& req, char **env);
};
