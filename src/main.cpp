#include "WebServ.hpp"

int main(int ac, char **av, char **env) {

    try
    {
        WebServ web(ac, av, env);
        web.runSocket();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    return (0);
}
