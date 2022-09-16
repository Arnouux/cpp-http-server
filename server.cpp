#include <iostream>
#include <winsock2.h>
#include <future>
#include <iostream>
#include <vector>
#include <string.h>
#include <algorithm>

const char header[] = "HTTP/1.1 200 OK\r\n"
"Content-Length: 100\r\n" 
"Connection: close\r\n"
"Content-type: text/html\r\n"
"\r\n";

const char html[] = "<html>\r\n"
"<head>\r\n"
"<title>Hello, world!</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"<h1>Hello, world!</h1>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

char response[] = "HTTP/1.1 200 OK\r\n"
"Content-Length: 100\r\n" 
"Connection: close\r\n"
"Content-type: text/html\r\n"
"\r\n"
"<html>\r\n"
"<head>\r\n"
"<title>Hello, world!</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"<h1>Hello, world!</h1>\r\n"
"</body>\r\n"
"</html>\r\n\r\n";

std::vector<std::string> addresses;

int main() {
    std::cout << "hello" << std::endl;

	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

    SOCKADDR_IN address, client_addr;

    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_family = AF_INET;
    address.sin_port = htons(5555);
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    bind(server, reinterpret_cast<SOCKADDR *>(&address), sizeof(address));
    if (listen(server, 0)) {
        std::cout << "Error on listen" << std::endl;
        return 1;
    }

    int client_addr_size = sizeof(client_addr);
    char buf[1024] = { 0 };
    for(;;) {
		SOCKET client;

		if ((client = accept(server, reinterpret_cast<SOCKADDR *>(&client_addr), &client_addr_size)) != INVALID_SOCKET)
		{
            std::cout << "client accepted:" << inet_ntoa(client_addr.sin_addr) << std::endl;
            //if(std::find(addresses.begin(), addresses.end(), inet_ntoa(client_addr.sin_addr)) != addresses.end()) {
            if(addresses.size() > 2) {
                // todo add timer per address for anti spam
                std::cout << addresses.size() << std::endl;
            } else {

                int val = recv(client, buf, 1024, 0);
                // if Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
                // elif Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
                
                addresses.push_back(inet_ntoa(client_addr.sin_addr));

                // todo append header + current response
                send(client, response, strlen(response)-1, 0);
            }
        }

		const auto last_error = WSAGetLastError();
		
		if(last_error > 0)
		{
			std::cout << "Error: " << last_error << std::endl;
		}
    }
    
}