#pragma once

#include <vector>
#include <string>
#include <unistd.h>
#include <stdexcept>
#include <sys/stat.h>
#include "statusCode.hpp"

class Location
{
private:
	bool		_cgi;
	bool		_get;
	bool		_post;
	bool		_delete;
	bool		_redirect;
	std::string	_root;
	std::string	_route;
	std::string	_index;
	std::string	_cgiExec;
	std::pair<int, std::string> _return;

public:
	Location();
	Location(const Location& loc);
	Location& operator=(const Location& loc);

	bool	isCGI();
	bool	canGet();
	bool	canPost();
	bool	haveRoot();
	bool	canDelete();
	bool	isRedirect();
	std::string	getRoot();
	std::string	getExec();
	std::string	getRoute();
	std::string	getIndex();
	std::string	getReturnLocation();
	std::string	getFullIndex(std::string serverRoot);

	int		getReturnCode();

	void	resetMethods();
	void	setGet(bool value);
	void	setPost(bool value);
	void	setDelete(bool value);
	void	setRoot(std::string	root);
	void	setRoute(std::string route);
	void	setIndex(std::string index);
	void	setReturn(int code, std::string link);
	void	setCgiPath(std::string path);

	void	checkValues(std::string serverRoot);
};
