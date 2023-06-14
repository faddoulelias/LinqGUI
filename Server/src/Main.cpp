#include <iostream>
#include "./headers/LinqServer.hpp"

int main()
{
	LinqServer::LinqServerThread *server_thread = new LinqServer::LinqServerThread("127.0.0.1", 3000);
	server_thread->startThread([](LinqServer::ServerConnection *)
							   { std::cout << "Server started!" << std::endl; });

	while (true)
		;
}
