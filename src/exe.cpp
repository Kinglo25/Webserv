#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <stdexcept>
#include <sys/wait.h>

#include "Location.hpp"
#include "HttpRequest.hpp"

# define TIMEOUT 5
# define BUFFER_SIZE 1024

std::string executeCgi(Location& loc, std::string serverRoot, char **env)
{
	int pipe_out[2];

	std::string	script_exec = loc.getExec();
	std::string script_path = loc.getFullIndex(serverRoot);

	char* const av[] = {const_cast<char*>(script_exec.c_str()), const_cast<char*>(script_path.c_str()), NULL};

	if (pipe(pipe_out) == -1)
	{
		perror("pipe");
		return "500 Internal Server Error";
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return "500 Internal Server Error";
	}

	if (!pid)
	{
		close(pipe_out[0]);

		dup2(pipe_out[1], STDOUT_FILENO);

		close(pipe_out[1]);

		execve(av[0], av, env);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else 
	{
		close(pipe_out[1]);

		int status;
		struct timeval tv;
		tv.tv_sec = TIMEOUT;
		tv.tv_usec = 0;

		char buffer[BUFFER_SIZE];
		std::string response;
		ssize_t bytes_read;

		// checking timeout
		while (true)
		{
			fd_set readfds;
			FD_ZERO(&readfds);
			FD_SET(pipe_out[0], &readfds);

			pid_t result = waitpid(pid, &status, WNOHANG);
			if (result == -1)
			{
				delete [] env;
				close(pipe_out[0]);
				throw HttpRequestException(INTERNAL_SERVER_ERROR, getStatusCodeName(INTERNAL_SERVER_ERROR));
			}
			else if (result > 0)
			{
				if (WIFEXITED(status))
				{
					if (WEXITSTATUS(status) == EXIT_SUCCESS)
						break;
					else
					{
						delete [] env;
						close(pipe_out[0]);
						throw HttpRequestException(INTERNAL_SERVER_ERROR, getStatusCodeName(INTERNAL_SERVER_ERROR));
					}
				}
				else
				{
					delete [] env;
					close(pipe_out[0]);
					throw HttpRequestException(INTERNAL_SERVER_ERROR, getStatusCodeName(INTERNAL_SERVER_ERROR));
				}
				break;
			}

			int ret = select(pipe_out[0] + 1, &readfds, NULL, NULL, &tv);
			if (ret == -1)			
			{
				delete [] env;
				close(pipe_out[0]);
				throw HttpRequestException(INTERNAL_SERVER_ERROR, getStatusCodeName(INTERNAL_SERVER_ERROR));
			}
			else if (ret == 0)
			{
				delete [] env;
				kill(pid, SIGKILL);
				waitpid(pid, &status, 0);
				close(pipe_out[0]);
				throw HttpRequestException(GATEWAY_TIMEOUT, getStatusCodeName(GATEWAY_TIMEOUT));
			}

			if (FD_ISSET(pipe_out[0], &readfds))
			{
				bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1);
				if (bytes_read > 0)
				{
					buffer[bytes_read] = '\0';
					response.append(buffer, bytes_read);
				}
			}
		}
		close(pipe_out[0]);
		return response;
	}
}
