#pragma once

#include <vector>
#include <fstream>

#include "utils.hpp"
#include "Server.hpp"
#include "statusCode.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#include <iostream>
#include <cstring> //memset
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>

#define BACKLOG 100 // maximum length to which the queue of pending connections for sockfd may grow
#define BUFFER_SIZE 2048000 // (4KB * 500 = 2MB)
#define MAX_EVENTS 100

class	WebServ
{
private:
	char	**_env;
	std::vector<Server> _servers;
	int					_epoll_fd;
	struct epoll_event	_event;
	struct epoll_event _eventsTab[MAX_EVENTS]; // Tableau pour stocker les événements
	std::vector<int> _server_fds; // Vector to store server file descriptors
	std::map<int, HttpRequest> _requestMap; // Map to store requests by file descriptor

public:
	WebServ(int ac, char **av, char **env);

	void	runSocket();
	void	closeAllSockets();
	bool	isServerFd(int fd);
	void	handleEvent(int i);
	void	setNonBlocking(int sockfd);
	void	parser(std::ifstream& file);
	Server&	searchServer(u_int32_t port);
	void	initSocket(const u_int32_t port);
	void	handleNewConnection(int event_fd);
	void	log(HttpRequest req, std::string res);
	void	handleError(const char* msg, int errnum);
	void	sendResponse(int fd, HttpRequest& request);
};
