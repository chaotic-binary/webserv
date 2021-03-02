//
// Created by Mahmud Jego on 2/22/21.
//

#include "includes.h"

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET; // ipV4
	sockaddr.sin_port = htons(8000); // htons is necessary to convert a number to // порт
	sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // htonl(INADDR_ANY);
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	// network byte order
	if (bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "port zanyat " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	//		inet_aton() запрещено !!!
	// inet_addr("") не запрещено

	if (listen(sockfd, 10) < 0)
	{
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	char msg[] = "hello from server\n";

	std::stringstream response_body;
	response_body << "<title>Test C++ HTTP Server</title>\n"
				  << "<h1>Test page</h1>\n"
				  << "<p>This is body of the test page...</p>\n"
				  << "<h2>Request headers</h2>\n"
				  << "<pre>" << msg << "</pre>\n"
				  << "<em><small>Test C++ Http Server</small></em>\n";

	// Формируем весь ответ вместе с заголовками
	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			 << "Version: HTTP/1.1\r\n"
			 << "Content-Type: text/html; charset=utf-8\r\n"
			 << "Content-Length: " << response_body.str().length()
			 << "\r\n\r\n"
			 << response_body.str();

	// Grab a connection from the queue
	int addrlen = sizeof(sockaddr);
	while (1)
	{
		int connection = accept(sockfd, (struct sockaddr *) &sockaddr, (socklen_t *) &addrlen);
		if (connection < 0) {
			std::cout << "Failed to grab connection. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}

		// Read from the connection
		char buffer[2048];
		int errr = recv(connection, buffer, 2048, MSG_PEEK);
		if (errr == -1)
			std::cout << "error" << std::endl;
//			int bytesRead = read(connection, buffer, 1024);
		std::cout << "The message was: " << buffer;
		bzero(buffer, 2048);
		// Send a message to the connection
		// write
		send(connection, response.str().c_str(), response.str().length(), 0);

		// Close the connections
		close(connection);
	}
	close(sockfd);
}