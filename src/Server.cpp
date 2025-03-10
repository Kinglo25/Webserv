#include "Server.hpp"

Server::Server(): _port(0), _name("localhost"), _root(""), _bodySize(1024)
{}

u_int32_t    Server::getPort()
{
	return (_port);
}

std::string Server::getName()
{
	return (_name);
}

std::string	Server::getRoot()
{
	return (_root);
}

std::string Server::getErrorPage(int code)
{
	std::map<int, std::string>::iterator it = _errPages.find(code);

	if (it == _errPages.end())
		return ("");
	return (it->second);
}

void    Server::addLocation(std::string path, Location loc)
{
	if (_locations.find(path) != _locations.end())
		throw std::runtime_error("another location already exist for " + path);
	if (!path.length() || path[0] != '/')
		throw std::runtime_error("location path should start with /");
	if (path.length() > 1 && path[path.length() - 1] == '/')
		path = path.substr(0, path.length() - 1);
	_locations[path] = loc;
}

void    Server::setPort(u_int32_t port)
{
	_port = port;
}

void    Server::setName(std::string name)
{
	_name = name;
}

void    Server::setRoot(std::string root)
{
	if (!_root.empty())
		throw std::runtime_error("server can have only one root");
	if (root[0] != '/')
		throw std::runtime_error("server root should start with /");
	if (root[root.length() - 1] == '/')
		root = root.substr(0, root.length() - 1);
	
	root = "." + root;

	struct stat statbuff;
	if (stat(root.c_str(), &statbuff) || !S_ISDIR(statbuff.st_mode))
		throw std::runtime_error("root should be a valid directory");

	_root = root;
}

void	Server::setBodySize(std::string body)
{
	size_t	i = 0;
	while (body[i] && isdigit(body[i]))
		i++;
	
	if (i == 0)
		std::runtime_error("client_max_body_size should be an integer");
	if (body.length() - i > 1 || (body.length() - i == 1 && body[i] != 'M' && body[i] != 'K'))
		std::runtime_error("client_max_body_size accept only M and K");

	std::string	num = body.substr(0, i);
	bool check = true;
	_bodySize = strToUl(num, check);
	if (!check)
		std::runtime_error("client_max_body_size should be an integer");
	if (body.length() - i == 1)
	{
		if (body[i] == 'M')
			_bodySize *= MEGA;
		else
			_bodySize *= KILO;
	}
	if (_bodySize > MAX_BODY_SIZE)
		std::runtime_error("client_max_body_size should not be greater than " + to_string(MAX_BODY_SIZE));
}

void    Server::addErrorPage(int code, std::string path)
{
	if (path[0] != '/')
		path[0] = '/';
	if (_errPages.find(code) != _errPages.end())
		throw std::runtime_error("already exists");
	size_t pos = path.find(".html");
	if (pos == std::string::npos || pos != path.length() - 5)
		throw std::runtime_error("error page should be an html file");
	_errPages[code] = path;
}

void    Server::checkValues()
{
	if (_port == 0)
		throw std::runtime_error("port is not set");
	if (!_locations.size())
		throw std::runtime_error("server must contain at least one location");

	std::string	path;
	std::map<int, std::string>::iterator it = _errPages.begin();

	while (it != _errPages.end())
	{
		if (_root.empty())
			path = "." + it->second;
		else
			path = _root + it->second;
		if (access(path.c_str(), F_OK | R_OK))
			throw std::runtime_error(it->second + " error page doesn't exists");
		it->second = path;
		it++;
	}

	std::map<std::string, Location>::iterator lit = _locations.begin();
	while (lit != _locations.end())
	{
		lit->second.checkValues(_root);
		lit++;
	}
}

size_t	getLastSlash(std::string path)
{
	size_t i = 0;
	size_t j = 0;

	while (i < path.length())
	{
		if (path[i] == '/')
			j = i;
		i++;
	}
	return (j);
}

std::string	getFileType(std::string file)
{
	size_t i = file.length() - 1;
	while (i > 0)
	{
		if (file[i] == '.')
			break ;
		i--;
	}
	if (i == file.length() - 1)
		throw HttpRequestException(NOT_IMPLEMENTED, getStatusCodeName(NOT_IMPLEMENTED));
	std::string	type = file.substr(i + 1);
	return (type);
}

void	addFileToBody(HttpResponse& res, std::string file)
{
	if (isDir(file))
	{
		std::string f = file;
		if (file[file.length() - 1] != '/')
			f += "/";
		f += "index.html";
		if (access(f.c_str(), F_OK))
			throw HttpRequestException(FORBIDDEN, getStatusCodeName(FORBIDDEN));
	}
	if (access(file.c_str(), F_OK))
		throw HttpRequestException(NOT_FOUND, getStatusCodeName(NOT_FOUND));

	if (isDir(file) || access(file.c_str(), F_OK | R_OK))
		throw HttpRequestException(FORBIDDEN, getStatusCodeName(FORBIDDEN));

	std::ifstream	ifile(file.c_str());
	if (ifile.fail() || ifile.eof())
		throw HttpRequestException(FORBIDDEN, getStatusCodeName(FORBIDDEN));

	std::string	content (
		(std::istreambuf_iterator<char>(ifile)),
		std::istreambuf_iterator<char>()
	);

	ifile.close();

	std::string	type = getFileType(file);
	if (type == "html" || type == "css")
		res.setHeader("Content-Type", "text/" + type);
	else if (type == "js")
		res.setHeader("Content-Type", "text/javascript");
	else if (type == "jpeg" || type == "jpg" || type == "png")
		res.setHeader("Content-Type", "image/" + type);
	else
		res.setHeader("Content-Type", "text/" + type);
	res.setHeader("Content-Length", to_string(content.length()));
	res.setBody(content);
}

void	Server::addErrorPageToBody(HttpResponse& res, int code)
{
	std::string	file = _root + _errPages[code];
	if (_root.empty())
		file = "." + _errPages[code];
	std::ifstream	ifile(file.c_str());
	if (ifile.fail() || ifile.eof())
		throw std::exception();

	std::string	content (
		(std::istreambuf_iterator<char>(ifile)),
		std::istreambuf_iterator<char>()
	);

	ifile.close();

	res.setHeader("Content-Type", "text/html");
	res.setHeader("Content-Length", to_string(content.length()));
	res.setBody(content);
}

void	addDefaultErrorPageToBody(HttpResponse& res, int code, std::string name)
{
	std::string	file = GENERAL_ERROR_PAGE;

	std::ifstream	ifile(file.c_str());
	if (ifile.fail() || ifile.eof())
		throw std::exception();

	std::string	content (
		(std::istreambuf_iterator<char>(ifile)),
		std::istreambuf_iterator<char>()
	);

	ifile.close();

	content.replace(content.find("{{ name }}"), 10, name);
	content.replace(content.find("{{ name }}"), 10, name);
	content.replace(content.find("{{ code }}"), 10, to_string(code));
	content.replace(content.find("{{ code }}"), 10, to_string(code));

	res.setHeader("Content-Type", "text/html");
	res.setHeader("Content-Length", to_string(content.length()));

	res.setBody(content);
}

Location&	Server::findLocation(std::string& path)
{
	while (path.length())
	{
		if (_locations.find(path) != _locations.end())
			return (_locations[path]);
		if (path.length() == 1)
			break ;
		if (path[path.length() - 1] == '/')
			path = path.substr(0, getLastSlash(path));
		else
			path = path.substr(0, getLastSlash(path) + 1);
	}
	throw HttpRequestException(NOT_FOUND, getStatusCodeName(NOT_FOUND));
}

char **addEnvironmentVariables(char **env, const std::vector<std::string>& newVars)
{
    int envCount = 0;
    while (env[envCount] != NULL)
        envCount++;

    char **newEnv = new char*[envCount + newVars.size() + 1];

    for (int i = 0; i < envCount; ++i)
        newEnv[i] = env[i];

    for (size_t i = 0; i < newVars.size(); ++i)
        newEnv[envCount + i] = const_cast<char*>(newVars[i].c_str());

    newEnv[envCount + newVars.size()] = NULL;

    return newEnv;
}

std::string	Server::execute(HttpRequest& req, char **env)
{
	std::string	header;

	try
	{
		header = req.getHeader("Content-Length");
		if (!header.empty())
		{
			bool check = true;
			unsigned long length = strToUl(header, check);
			(void)check;
			if (length > _bodySize)
				throw HttpRequestException(CONTENT_TOO_LARGE, getStatusCodeName(CONTENT_TOO_LARGE));
		}
		std::string	path = req.getPath();
		Location&	loc = findLocation(path);

		if ((req.getMethod() == "GET" && !loc.canGet()) || (req.getMethod() == "POST" && !loc.canPost()) || (req.getMethod() == "DELETE" && !loc.canDelete()))
			throw HttpRequestException(METHOD_NOT_ALLOWED, getStatusCodeName(METHOD_NOT_ALLOWED));

		if (loc.isCGI())
		{
			std::vector<std::string> newVars;
			newVars.push_back("BODY=" + req.getBody());
			newVars.push_back("METHOD=" + req.getMethod());
			newVars.push_back("PATH_INFO=" + req.getPath());
			newVars.push_back("FILENAME=" + req.getFileName());
			newVars.push_back("COOKIE=" + req.getHeader("Cookie"));

			char **newEnv = addEnvironmentVariables(env, newVars);
			std::string res = executeCgi(loc, _root, newEnv); 
			delete [] newEnv;
			return(res);
		}
		else
		{
			HttpResponse res;

			header = req.getHeader("Connection");

			if (!header.empty())
				res.setHeader("Connection", header);
			else
			{
				res.setHeader("Connection", "close");
				req.setHeader("Connection", "close");
			}

			if (loc.isRedirect())
			{
				res.setStatusCode(loc.getReturnCode());
				res.setStatusMsg(getStatusCodeName(loc.getReturnCode()));
				res.setHeader("Location", loc.getReturnLocation() + req.getQuery());
				res.setHeader("Connection", "close");
				req.setHeader("Connection", "close");
				return (res.toString());
			}

			path = req.getPath();
			if (path.length() > 1 && path[path.length() - 1] == '/')
				path = path.substr(0, path.length() - 1);
			if (path == loc.getRoute())
				path = loc.getFullIndex(_root);
			else
			{
				if (loc.haveRoot())
					path = loc.getRoot() + path;
				else
					path = _root + path;
			}
			res.setStatusCode(OK);
			res.setStatusMsg(getStatusCodeName(OK));
			addFileToBody(res, path);
			return (res.toString());
		}
	}
	catch (const HttpRequestException& e)
	{
		HttpResponse res(e.code(), e.what());
		header = req.getHeader("Connection");
		if (!header.empty())
			res.setHeader("Connection", header);
		else
		{
			res.setHeader("Connection", "close");
			req.setHeader("Connection", "close");
		}
		try
		{
			if (getErrorPage(e.code()).empty())
				addDefaultErrorPageToBody(res, e.code(), e.what());
			else
				addErrorPageToBody(res, e.code());
		}
		catch(const std::exception& ee)
		{
			header = "<h1>error: " + std::string(ee.what()) + "</h1>";
			res.setHeader("Content-Length", to_string(header.length()));
			res.setHeader("Content-Type", "text/html");
			res.setBody(header);
		}
		return (res.toString());
	}
	catch (const std::exception& e)
	{
		HttpResponse res(NOT_IMPLEMENTED, getStatusCodeName(NOT_IMPLEMENTED));
		header = req.getHeader("Connection");
		if (!header.empty())
			res.setHeader("Connection", header);
		else
		{
			res.setHeader("Connection", "close");
			req.setHeader("Connection", "close");
		}
		try
		{
			if (getErrorPage(NOT_IMPLEMENTED).empty())
				addDefaultErrorPageToBody(res, NOT_IMPLEMENTED, e.what());
			else
				addErrorPageToBody(res, NOT_IMPLEMENTED);
		}
		catch(const std::exception& ee)
		{
			header = "<h1>error: " + std::string(e.what()) + "</h1>";
			res.setHeader("Content-Length", to_string(header.length()));
			res.setHeader("Content-Type", "text/html");
			res.setBody(header);
		}
		return (res.toString());
	}
}
