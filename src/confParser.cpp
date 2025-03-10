#include "confParser.hpp"

int	isrule(int c)
{
	return (isalpha(c) || c == '_');
}

int	isdir(int c)
{
	return (isrule(c) || c == '-');
}

int	isfile(int c)
{
	return (isdir(c) || isdigit(c) || c == '/' || c == '.');
}

int	isroute(int c)
{
	return (isdir(c) || isdigit(c) || c == '/');
}

int	islink(int c)
{
	return (isfile(c) || c == ':');
}

int	isother(int c)
{
	return (!isfile(c) && !isspace(c) && !islink(c));
}

int	isany(int c)
{
	return (!isspace(c));
}

std::string	getNextElem(std::string& line, int& i, int (*f)(int))
{
	int k;

	while (line[i] && isspace(line[i]))
		i++;
	k = i;
	while (line[i] && f(line[i]))
		i++;
	return (line.substr(k, i - k));
}

std::string	addLine(int line, std::string message)
{
	std::ostringstream oss;

	oss << "line " << line;
	oss << ": " << message;
	return (oss.str());
}

void	closeAndThrow(std::string message, int line, std::ifstream& file)
{
	file.close();
	throw std::runtime_error(addLine(line, message));
}

void	checkEndOfLine(std::string& line, int& i, int j, bool semicolon, std::ifstream& file)
{
	std::string rest = getNextElem(line, i, isany);

	if (semicolon && (!rest.length() || rest[0] != ';'))
		closeAndThrow("no semicolon at end of line", j, file);
	if (semicolon && rest.length() > 1 && rest[1] != '#')
		closeAndThrow("bad end of line 1", j, file);
	if (semicolon && rest.length() > 1 && rest[1] == '#')
		return ;
	if (rest.length() && rest[0] == '#')
		return ;
	rest = getNextElem(line, i, isany);
	if (!rest.length() || rest[0] == '#')
		return ;
	closeAndThrow("bad end of line 3", j, file);
}

void	endOfServerCheck(std::string& line, int&i, int& j, int& serverIdx, Server server, std::vector<u_int32_t>& ports,
	std::vector<std::string>& names, std::string& elem, bool& isServer, std::ifstream& ifile)
{
	if ((elem.length() > 1 && elem[1] != '#') || !isServer)
		closeAndThrow("bad character", j, ifile);
	serverIdx++;
	isServer = false;
	checkEndOfLine(line, i, j, false, ifile);
	try
	{
		server.checkValues();
	}
	catch(const std::exception& e)
	{
		closeAndThrow(e.what(), j, ifile);
	}
	if (std::find(ports.begin(), ports.end(), server.getPort()) != ports.end())
		closeAndThrow("port " + to_string(server.getPort()) + " is already in use", j, ifile);
	if (std::find(names.begin(), names.end(), server.getName()) != names.end())
		closeAndThrow("server name " + server.getName() + " is already in use", j, ifile);
	ports.push_back(server.getPort());
	names.push_back(server.getName());
}

void	serverCheck(bool& server, std::vector<Server>& servers, std::string& line, int& i, int& j, std::ifstream& ifile)
{
	server = true;
	servers.push_back(Server());

	std::string elem = getNextElem(line, i, isother);
	if (!elem.length() || elem[0] != '{')
		closeAndThrow("syntax error: `{` is missing after server", j, ifile);
	if (elem.length() > 1 && elem[1] != '#')
		closeAndThrow("bad end of line", j, ifile);
	checkEndOfLine(line, i, j, false, ifile);
}

void	bodySizeCheck(std::string line, int& i, int& j, Server& server, std::ifstream& ifile)
{
	std::string	elem = getNextElem(line, i, isalnum);

	if (elem.empty())
		closeAndThrow("bad value for client_max_body_size", j, ifile);
	try
	{
		server.setBodySize(elem);
	}
	catch(const std::exception& e)
	{
		closeAndThrow(e.what(), j, ifile);
	}
	checkEndOfLine(line, i, j, true, ifile);
}

void	listenCheck(std::string& line, int& i, int&j, Server& server, std::ifstream& ifile)
{
	u_int32_t	port;
	std::string	elem;

	elem = getNextElem(line, i, isdigit);
	if (!elem.length())
		closeAndThrow("bad value for listen", j, ifile);
	bool check = true;
	port = static_cast<u_int32_t>(strToUl(elem, check));
	if (!check || port < 1024 || port > 49151)
		closeAndThrow("port should be in the range of 1024 and 49151", j, ifile);
	checkEndOfLine(line, i, j, true, ifile);
	server.setPort(port);
}

void	serverNameCheck(std::string& line, int& i, int& j, Server& server, std::ifstream& ifile)
{
	std::string	elem;

	elem = getNextElem(line, i, islink);
	if (!elem.length())
		closeAndThrow("no server name", j, ifile);
	server.setName(elem);
	checkEndOfLine(line, i, j, true, ifile);
}

void	serverRootCheck(std::string& line, int& i, int& j, Server& server, std::ifstream& ifile)
{
	std::string elem = getNextElem(line, i, isroute);

	if (!elem.length())
		closeAndThrow("server root isn't set", j, ifile);
	try
	{
		server.setRoot(elem);
	}
	catch(const std::exception& e)
	{
		closeAndThrow(e.what(), j, ifile);
	}
	checkEndOfLine(line, i, j, true, ifile);
	
}

void	errorPageCheck(std::string& line, int&i, int& j, Server& server, std::ifstream& ifile)
{
	size_t	k;
	std::string	elem;
	std::vector<int> codes;

	while (line[i])
	{
		elem = getNextElem(line, i, isdigit);
		if (!elem.length() || line[i] == '.')
			break ;
		if (!isValidStatusCode(atoi(elem.c_str())))
			closeAndThrow("bad error code", j, ifile);
		codes.push_back(static_cast<int>(atoi(elem.c_str())));
	}
	if (!codes.size())
		closeAndThrow("no code for error page", j, ifile);
	if (!elem.length())
		elem = getNextElem(line, i, isfile);
	else
		elem += getNextElem(line, i, isfile);
	checkEndOfLine(line, i, j, true, ifile);

	k = 0;
	while (k < codes.size())
	{
		try
		{
			server.addErrorPage(codes[k], elem);
		}
		catch (const std::exception& e)
		{
			closeAndThrow(to_string(codes[k]) + " " + e.what(), j, ifile);
		}
		k++;
	}
}

void	locationCheck(std::string& line, int&i, int& j, Server& server, std::ifstream& ifile)
{
	int	tmp;
	bool bloc;
	Location loc;
	std::string	path;

	std::string	elem = getNextElem(line, i, isroute);

	if (!elem.length() || elem[0] != '/')
		closeAndThrow("location need a route", j, ifile);
	path = elem;
	elem = getNextElem(line, i, isother);
	if (!elem.length() || elem[0] != '{' || (elem.length() > 1 && elem[1] != '#'))
		closeAndThrow("syntax error: `{` is missing after location", j, ifile);
	checkEndOfLine(line, i, j, false, ifile);

	bloc = true;
	j++;
	while (getline(ifile, line))
	{
		i = 0;
		elem = getNextElem(line, i, isother);
		if (elem.length() && elem[0] == '#' && ++j)
			continue ;
		else if (elem.length() > 1 && elem[0] == '}' && elem[1] != '#')
			closeAndThrow("bad character", j, ifile);
		else if (elem.length() && elem[0] == '}')
		{
			checkEndOfLine(line, i, j, false, ifile);
			bloc = false;
			break ;
		}
		if (elem.length())
			closeAndThrow("bad character", j, ifile);

		elem = getNextElem(line, i, isrule);
		if (!elem.length() && ++j)
			continue ;
		else if (elem == "root")
		{
			elem = getNextElem(line, i, isroute);
			if (!elem.length())
				closeAndThrow("root need a route", j, ifile);
			try
			{
				loc.setRoot(elem);
			}
			catch(const std::exception& e)
			{
				closeAndThrow(e.what(), j, ifile);
			}			
			checkEndOfLine(line, i, j, true, ifile);
		}
		else if (elem == "return")
		{
			elem = getNextElem(line, i, isdigit);
			if (elem.length() != 3 || elem[0] != '3')
				closeAndThrow("return need a valid redirect code", j, ifile);
			tmp = atoi(elem.c_str());
			switch (tmp)
			{
			case 300:
			case 301:
			case 302:
			case 303:
			case 304:
			case 307:
			case 308:
				break;
			default:
				closeAndThrow("return need a valid redirect code", j, ifile);
				break;
			}
			elem = getNextElem(line, i, islink);
			if (!elem.length())
				closeAndThrow("return need a valid link", j, ifile);
			checkEndOfLine(line, i, j, true, ifile);
			try
			{
				loc.setReturn(tmp, elem);
			}
			catch(const std::exception& e)
			{
				closeAndThrow("location can have only one return", j, ifile);
			}			
		}
		else if (elem == "index")
		{
			elem = getNextElem(line, i, isfile);
			if (!elem.length())
				closeAndThrow("index need a file", j, ifile);
			try
			{
				loc.setIndex(elem);
			}
			catch(const std::exception& e)
			{
				closeAndThrow(e.what(), j, ifile);
			}
			checkEndOfLine(line, i, j, true, ifile);
		}
		else if (elem == "limit_except")
		{
			loc.resetMethods();
			tmp = 0;
			while (tmp < 3)
			{
				elem = getNextElem(line, i, isalpha);
				if (!elem.length())
					break ;
				if (elem == "GET")
					loc.setGet(true);
				else if (elem == "POST")
					loc.setPost(true);
				else if (elem == "DELETE")
					loc.setDelete(true);
				else
					closeAndThrow("limit except have to be followed by valid methods", j, ifile);
				tmp++;
			}
			if (!tmp || (!loc.canGet() && !loc.canPost() && !loc.canDelete()))
				closeAndThrow("limit except have to be followed by valid methods", j, ifile);
			checkEndOfLine(line, i, j, true, ifile);
		}
		else
			closeAndThrow(elem + " is not a valid keyword", j, ifile);
		j++;
	}
	if (bloc)
		closeAndThrow("location not closed", j, ifile);

	try
	{
		loc.setRoute(path);
	}
	catch(const std::exception& e)
	{
		closeAndThrow(e.what(), j, ifile);
	}
	try
	{
		server.addLocation(path, loc);
	}
	catch(const std::exception& e)
	{
		closeAndThrow(e.what(), j, ifile);
	}
	
}
