#include "Location.hpp"

Location::Location(): _cgi(false), _get(true), _post(true), _delete(true), _redirect(false), _route(""), _index(""), _cgiExec("") {}

Location::Location(const Location& loc)
{
	_cgi = loc._cgi;
	_get = loc._get;
	_post = loc._post;
	_delete = loc._delete;
	_redirect = loc._redirect;
	_root = loc._root;
	_route = loc._route;
	_index = loc._index;
	_cgiExec = loc._cgiExec;
	_return = loc._return;
}

Location& Location::operator=(const Location& loc)
{
	if (this != &loc)
	{
		_cgi = loc._cgi;
		_get = loc._get;
		_post = loc._post;
		_delete = loc._delete;
		_redirect = loc._redirect;
		_root = loc._root;
		_route = loc._route;
		_index = loc._index;
		_cgiExec = loc._cgiExec;
		_return = loc._return;
	}
	return (*this);
}

bool	Location::isCGI()
{
	return (_cgi);
}

bool	Location::canGet()
{
	return (_get);
}

bool	Location::canPost()
{
	return (_post);
}

bool	Location::haveRoot()
{
	return (!_root.empty());
}

bool	Location::canDelete()
{
	return (_delete);
}

bool	Location::isRedirect()
{
	return (_redirect);
}

std::string	Location::getRoot()
{
	return (_root);
}

std::string	Location::getRoute()
{
	return (_route);
}

std::string	Location::getIndex()
{
	return (_index);
}

std::string	Location::getFullIndex(std::string serverRoot)
{
	std::string root = _root;
	std::string index = _index;

	if (root.empty())
		root = serverRoot;

	if (index.empty())
		index = "index.html";

	if (_route[_route.length() - 1] == '/')
		return (root + _route + index);
	return (root + _route + "/" + index);
}

int		Location::getReturnCode()
{
	return (_return.first);
}

std::string	Location::getReturnLocation()
{
	return (_return.second);
}

std::string Location::getExec()
{
	return (_cgiExec);
}

void	Location::setRoot(std::string	root)
{
	if (!_root.empty())
		throw std::runtime_error("location can have only one root");
	if (root[0] != '/')
		throw std::runtime_error("location root should start with /");
	if (root.length() > 1 && root[root.length() - 1] == '/')
		root = root.substr(0, root.length() - 1);

	root = "." + root;

	struct stat statbuff;
	if (stat(root.c_str(), &statbuff) || !S_ISDIR(statbuff.st_mode))
		throw std::runtime_error("root should be a valid directory");

	_root = root;
}

void	Location::setIndex(std::string index)
{
	if (!_index.empty())
		throw std::runtime_error("location can have only one index");
	_index = index;

	if (_index[_index.length() - 1] == '/')
		throw std::runtime_error("location index shouldn't end with /");

	if (_index[0] == '/')
		index = index.substr(1);

	int i = 0;
	while (index[i] && index[i] != '.')
		i++;
	std::string tmp = index.substr(i + 1);
	if (tmp != "html" && tmp != "css")
	{
		_cgi = true;
		if (tmp == "php")
			_cgiExec = "/usr/bin/php";
		else if (tmp == "py")
			_cgiExec = "/usr/bin/python3";
		else
			_cgiExec = "bash";
	}
}

void	Location::setGet(bool value)
{
	_get = value;
}

void	Location::setPost(bool value)
{
	_post = value;
}

void	Location::setDelete(bool value)
{
	_delete = value;
}

void	Location::resetMethods()
{
	_get = false;
	_delete = false;
	_post = false;
}

void	Location::setRoute(std::string route)
{
	if (!route.length() || route[0] != '/')
		throw std::runtime_error("location route should start with /");
	_route = route;
}

void	Location::setReturn(int code, std::string link)
{
	if (_redirect)
		throw std::exception();
	_return = std::make_pair(code, link);
	_redirect = true;
}

void	Location::checkValues(std::string serverRoot)
{
	std::string	path;
	std::string root;

	if (_route.empty())
		throw std::runtime_error("location " + _route + " route can't be empty");
	if (_redirect && (!_root.empty() || !_index.empty()))
		throw std::runtime_error("location " + _route + " return can't be in the same location as root and index");
	if (!_get && !_post && !_delete)
		throw std::runtime_error("location " + _route + " need to have at least one http method");
	if (!_get && _redirect)
		throw std::runtime_error("location " + _route + " redirect location has to have a GET method");
	if (_redirect)
		return ;
	if (_root.empty() && serverRoot.empty())
		throw std::runtime_error("location " + _route + " root and index cannot be empty");

	root = _root;
	if (root.empty())
		root = serverRoot;
	path = root + _route;

	struct stat statbuff;
	if (stat(path.c_str(), &statbuff) || !S_ISDIR(statbuff.st_mode))
		throw std::runtime_error("location " + _route + " root / route should be a valid directory");		

	if (!_index.empty())
	{
		if (_route[_route.length() - 1] == '/')
			path = root + _route + _index;
		else
			path = root + _route + "/" + _index;

		if (stat(path.c_str(), &statbuff) || S_ISDIR(statbuff.st_mode) || access(path.c_str(), F_OK | R_OK))
			throw std::runtime_error("location " + _route + " index should be a valid file");
	}
}
