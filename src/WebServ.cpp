#include "WebServ.hpp"
#include "confParser.hpp"

WebServ::WebServ(int ac, char **av, char **env): _env(env)
{
	std::string		file;

	file = "./conf/default.conf";
	if (ac == 2)
		file = av[1];

	std::ifstream	ifile(file.c_str());
	if (ifile.fail() || ifile.eof())
		throw std::runtime_error("can't open " + file);
	parser(ifile);

	// Create epoll instance
	_epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (_epoll_fd == -1)
	{
		handleError("epoll_create failed", errno);
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < _servers.size(); i++)
		initSocket(_servers[i].getPort());
}

/**
 * @brief Initializes a socket for a given port and adds it to the epoll instance.
 * 
 * @param port The port number to bind the socket to.
 */
void	WebServ::initSocket(const u_int32_t port)
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 Internet protocols // ensure that data is not lost or duplicated.
	if (server_fd == -1)
	{
		handleError("socket creation failed", errno);
		closeAllSockets();
		exit(EXIT_FAILURE);
	}

	// Configurer le socket pour réutiliser l'adresse et le port
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) // Set socket options
	{
		handleError("setsockopt failed", errno);
		close(server_fd);
		closeAllSockets();
		exit(EXIT_FAILURE);
	}

	// Associer le socket à une adresse et un port
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port); //function converts the unsigned short integer hostshort from host byte order to network byte order

	// Binds a socket to a specific address and port
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		handleError("bind failed", errno);
		close(server_fd);
		closeAllSockets();
		exit(EXIT_FAILURE);
	}

	//Marks a socket as passive, indicating readiness to accept incoming connections
	if (listen(server_fd, BACKLOG) == -1)
	{
		handleError("listen failed", errno);
		close(server_fd);
		closeAllSockets();
		exit(EXIT_FAILURE);
	}

	// Configurer le socket principal en mode non bloquant
	setNonBlocking(server_fd);

	// Ajouter le socket principal à epoll
	_event.data.fd = server_fd;
	_event.events = EPOLLIN;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, server_fd, &_event) == -1)
	{
		handleError("epoll_ctl failed", errno);
		close(server_fd);
		closeAllSockets();
		exit(EXIT_FAILURE);
	}
	_server_fds.push_back(server_fd);
	std::cout << "Initialized socket on port " << port << " with fd " << server_fd << std::endl;
}

/**
 * @brief Handle errors by printing an error message.
 * 
 * @param msg The error message to print.
 * @param errnum The error number to print.
 */
void WebServ::handleError(const char* msg, int errnum)
{
	std::cerr << msg << ": " << strerror(errnum) << std::endl;
}

/**
 * @brief Configure a socket to be non-blocking.
 * 
 * This function sets the specified socket file descriptor to non-blocking mode.
 * In non-blocking mode, system calls that would normally block (e.g., read, write)
 * will return immediately with a partial result or an error indicating that the
 * operation would block.
 * 
 * @param sockfd The socket file descriptor to configure.
 */
void WebServ::setNonBlocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0); // fcntl = Manipulate file descriptor
	if (flags == -1)
	{
		handleError("fcntl(F_GETFL) failed", errno);
		close(sockfd);
		closeAllSockets();
		exit(EXIT_FAILURE);
	}
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		handleError("fcntl(F_SETFL) failed", errno);
		close(sockfd);
		closeAllSockets();
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Handle an epoll event for a specific file descriptor.
 * 
 * This function processes an epoll event for a given file descriptor. It reads data
 * from the file descriptor, parses the HTTP request, and prepares the HTTP response.
 * If the file descriptor is ready for reading, it reads the data and parses the request.
 * If the file descriptor is ready for writing, it prepares to send the response.
 * 
 * @param events The array of epoll events.
 * @param i The index of the current event in the events array.
 * @param _epoll_fd The epoll file descriptor.
 */
void WebServ::handleEvent(int i)
{
	char buffer[BUFFER_SIZE];
	std::string requestData;
	ssize_t bytes_read;

	while (true)
	{
		memset(buffer, 0, BUFFER_SIZE);
		bytes_read = read(_eventsTab[i].data.fd, buffer, BUFFER_SIZE - 1);
		if (bytes_read <= 0 && (requestData.length() == 0))
		{
			epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _eventsTab[i].data.fd, NULL);
			_requestMap.erase(_eventsTab[i].data.fd); // Remove request from map if client disconnects
			if (bytes_read <= 0)
			{
				close(_eventsTab[i].data.fd);
				_eventsTab[i].data.fd = -1;
				return;
			}
		}
		if (bytes_read > 0)
			requestData.append(buffer, bytes_read);
		else
			break;
	}

	try {
		// Parse the HTTP request && Store the request in the map
		_requestMap[_eventsTab[i].data.fd] = HttpRequest(requestData);

	}
	catch (const HttpRequestException& e) {
		
		HttpResponse response(e.code(), e.what());
		try
		{
			addDefaultErrorPageToBody(response, e.code(), e.what());
		}
		catch(const std::exception& error)
		{
			std::string content = "<h1>" + to_string(e.code()) + " " + e.what() + "</h1>";
			response.setHeader("Content-Length", to_string(content.length()));
			response.setHeader("Content-Type", "text/html");
			response.setBody(content);
			std::cerr << "127.0.0.1:xxxx - REQUEST ERROR" << std::endl;
		}
		std::string responseStr = response.toString();

		ssize_t bytes_sent = send(_eventsTab[i].data.fd, responseStr.c_str(), responseStr.size(), 0);

		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _eventsTab[i].data.fd, NULL);
		_requestMap.erase(_eventsTab[i].data.fd); // Remove request from map if parsing fails
		close(_eventsTab[i].data.fd);
		_eventsTab[i].data.fd = -1;

		if (bytes_sent == -1)
		{
			handleError("send failed", errno);
			closeAllSockets();
			exit(EXIT_FAILURE);
		}
		else
			return ;
	}
	catch (const std::exception& e) {
		
		HttpResponse response(INTERNAL_SERVER_ERROR, getStatusCodeName(INTERNAL_SERVER_ERROR));
		try
		{
			addDefaultErrorPageToBody(response, INTERNAL_SERVER_ERROR, getStatusCodeName(INTERNAL_SERVER_ERROR));
		}
		catch(const std::exception& error)
		{
			std::string content = "<h1>" + to_string<int>(INTERNAL_SERVER_ERROR) + " " + getStatusCodeName(INTERNAL_SERVER_ERROR) + "</h1>";
			response.setHeader("Content-Length", to_string(content.length()));
			response.setHeader("Content-Type", "text/html");
			response.setBody(content);
			std::cerr << "127.0.0.1:xxxx - REQUEST ERROR" << std::endl;
		}
		std::string responseStr = response.toString();

		ssize_t bytes_sent = send(_eventsTab[i].data.fd, responseStr.c_str(), responseStr.size(), 0);

		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _eventsTab[i].data.fd, NULL);
		_requestMap.erase(_eventsTab[i].data.fd); // Remove request from map if parsing fails
		close(_eventsTab[i].data.fd);
		_eventsTab[i].data.fd = -1;

		if (bytes_sent == -1)
		{
			handleError("send failed", errno);
			closeAllSockets();
			exit(EXIT_FAILURE);
		}
		else
			return ;
	}
	
	// Prepare to send the response
	struct epoll_event event;
	event.data.fd = _eventsTab[i].data.fd;
	event.events = EPOLLOUT;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, _eventsTab[i].data.fd, &event) == -1)
	{
		handleError("epoll_ctl: mod failed", errno);
		_requestMap.erase(_eventsTab[i].data.fd); // Remove request from map if epoll_ctl fails
		closeAllSockets();
		exit(EXIT_FAILURE);
	}
}

void WebServ::log(HttpRequest req, std::string res)
{
	std::string code = "";

	if (res.length() >= 12)
		code = res.substr(9, 3);

	std::cout << "127.0.0.1:" << req.getPort();
	std::cout << " - " << req.getMethod() << " ";
	std::cout << req.getRequest() << " - " << code << std::endl;
}

/**
 * @brief Send an HTTP response to the client.
 * 
 * This function creates an HTTP response based on the provided HttpRequest object
 * and sends it to the client. If the send operation fails, it handles the error
 * and closes the connection. If the connection should be kept alive, it modifies
 * the epoll event to continue reading data from the client.
 * 
 * @param fd The file descriptor of the client socket.
 * @param _epoll_fd The epoll file descriptor.
 * @param request The HttpRequest object containing the client's request.
 */
void WebServ::sendResponse(int fd, HttpRequest& request)
{
	Server &serv = searchServer(request.getPort());

	// Send response
	std::string responseStr;
	try {
		responseStr = serv.execute(request, _env);
	}
	catch (std::exception& e) {
		HttpResponse response(500, "Internal Server Error");
		try
		{
			addDefaultErrorPageToBody(response, 500, "Internal Server Error");
		}
		catch(const std::exception& error)
		{
			response.setHeader("Content-Length", "34");
			response.setHeader("Content-Type", "text/html");
			response.setBody("<h1>500 Internal Server Error</h1>");
		}
		responseStr = response.toString();
	}
	log(request, responseStr);
	ssize_t bytes_sent = send(fd, responseStr.c_str(), responseStr.size(), 0);
	if (bytes_sent == -1)
	{
		std::cerr << "send failed" << std::endl;
		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		_requestMap.erase(fd); // Remove request from map if send fails
		close(fd);
		fd = -1;
		closeAllSockets();
		exit(EXIT_FAILURE);
	}
	else
	{
		// Check the Connection header
		if (request.getHeader("Connection") == "close" || request.getHeader("Connection") == "" || bytes_sent == 0)
		{
			close(fd);
			fd = -1;
			epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
			_requestMap.erase(fd); // Remove request from map after sending response
		}
		else
		{
			// Switch back to EPOLLIN to read more data
			struct epoll_event event;
			event.data.fd = fd;
			event.events = EPOLLIN;
			if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &event) == -1)
			{
				handleError("epoll_ctl: mod", errno);
				_requestMap.erase(fd); // Remove request from map if epoll_ctl fails
				close(fd);
				fd = -1;
				closeAllSockets();
				exit(EXIT_FAILURE);
			}
		}
	}
}

/**
 * @brief Runs the main loop to handle events on the sockets.
 */
void WebServ::runSocket()
{
	while (true)
	{
		int num_fds = epoll_wait(_epoll_fd, _eventsTab, MAX_EVENTS, -1);
		if (num_fds == -1)
		{
			handleError("epoll_wait failed", errno);
			closeAllSockets();
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i < num_fds; i++)
		{
			if (isServerFd(_eventsTab[i].data.fd))
			{
				handleNewConnection(_eventsTab[i].data.fd);
			}
			else if (_eventsTab[i].events & EPOLLIN)
				handleEvent(i);
			else if (_eventsTab[i].events & EPOLLOUT)
			{
				if (_requestMap.find(_eventsTab[i].data.fd) != _requestMap.end())
					sendResponse(_eventsTab[i].data.fd, _requestMap[_eventsTab[i].data.fd]);
				else
				{
					_requestMap.erase(_eventsTab[i].data.fd); // Remove request from after sending response
					close(_eventsTab[i].data.fd);
				}
			}
		}
	}
	closeAllSockets();
}

/**
 * @brief Checks if the given file descriptor is a server socket.
 * 
 * @param fd The file descriptor to check.
 * @return true If the file descriptor is a server socket.
 * @return false If the file descriptor is not a server socket.
 */
bool WebServ::isServerFd(int fd)
{
	return std::find(_server_fds.begin(), _server_fds.end(), fd) != _server_fds.end();
}

/**
 * @brief Handles a new connection on a server socket.
 * 
 * @param event_fd The file descriptor of the server socket.
 */
void WebServ::handleNewConnection(int event_fd)
{
	struct sockaddr_in client_address;
	socklen_t client_len = sizeof(client_address);
	int client_fd = accept(event_fd, (struct sockaddr *)&client_address, &client_len);
	if (client_fd == -1)
	{
		handleError("accept failed", errno);
		return; //ERROR system?
	}

	setNonBlocking(client_fd);

	_event.data.fd = client_fd;
	_event.events = EPOLLIN;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_event) == -1)
	{
		handleError("epoll_ctl: mod", errno);
		close(client_fd);
		client_fd = -1;
		closeAllSockets();
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Closes all server sockets and the epoll file descriptor.
 */
void WebServ::closeAllSockets()
{
	for (size_t i = 0; i < _server_fds.size(); i++)
	{
		if (_server_fds[i] != -1)
		{
			close(_server_fds[i]);
			_server_fds[i] = -1; // pour etre sure que le descripteur de fichier est fermé
		}
	}
	if (_epoll_fd != -1)
	{
		close(_epoll_fd);
		_epoll_fd = -1; // idem
	}
}

void	WebServ::parser(std::ifstream& ifile)
{
	int	i;
	int	j;

	bool	server;

	std::string	line;
	std::string	elem;

	int	serverIdx = 0;

	std::vector<u_int32_t> ports;
	std::vector<std::string> names;

	j = 1;
	server = false;
	while (getline(ifile, line))
	{
		i = 0;
		elem = getNextElem(line, i, isother);
		if (elem.length() && elem[0] == '#' && ++j)
			continue ;
		else if (elem.length() && elem[0] == '}')
			endOfServerCheck(line, i, j, serverIdx, _servers[serverIdx], ports, names, elem, server, ifile);
		else if (elem.length())
			closeAndThrow("bad character", j, ifile);

		elem = getNextElem(line, i, isrule);

		if (!elem.length() && ++j)
			continue ;
		if (elem == "server" && server)
			closeAndThrow("cannot add a server in another server", j, ifile);
		else if (elem == "server" && !server)
			serverCheck(server, _servers, line, i, j, ifile);
		else if (elem == "client_max_body_size" && !server)
			closeAndThrow("client_max_body_size must be in a server", j, ifile);
		else if (elem == "client_max_body_size")
			bodySizeCheck(line, i, j, _servers[serverIdx], ifile);
		else if (elem == "listen" && !server)
			closeAndThrow("listen must be in a server", j, ifile);
		else if (elem == "listen")
			listenCheck(line, i, j, _servers[serverIdx], ifile);
		else if (elem == "server_name" && !server)
			closeAndThrow("server_name must be in a server", j, ifile);
		else if (elem == "server_name")
			serverNameCheck(line, i, j, _servers[serverIdx], ifile);
		else if (elem == "root" && !server)
			closeAndThrow("root must be in a server", j, ifile);
		else if (elem == "root")
			serverRootCheck(line, i, j, _servers[serverIdx], ifile);
		else if (elem == "error_page" && !server)
			closeAndThrow("error_page must be in a server", j, ifile);
		else if (elem == "error_page")
			errorPageCheck(line, i, j, _servers[serverIdx], ifile);
		else if (elem == "location" && !server)
			closeAndThrow("location must be in a server", j, ifile);
		else if (elem == "location")
			locationCheck(line, i, j, _servers[serverIdx], ifile);
		else
			closeAndThrow(elem + " is not a valid keyword", j, ifile);
		j++;
	}
	if (server)
		closeAndThrow("server not closed", j, ifile);
	if (_servers.size() == 0)
		closeAndThrow("no server found", j, ifile);
	ifile.close();
}

Server&	WebServ::searchServer(u_int32_t port)
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i].getPort() == port)
			return (_servers[i]);
	}
	throw std::runtime_error("couldn't find a server with this port: " + to_string(port));
}

