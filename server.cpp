#include <iostream>
#include <winsock2.h>
#include <future>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <time.h>

// todo content-length = file size

std::vector<char> getHeader(int size, std::string content_type) {
    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    std::string s = "HTTP/1.1 200 OK\r\n"
"Content-Length: " + std::to_string(size-3) + "\r\n" 
"Connection: close\r\n"
"Date: " + buf + "\r\n"
"Content-type: " + content_type + "\r\n"
"\r\n";
    std::vector<char> vec(s.begin(), s.end());
    return vec;
}

std::vector<char> getDataWithHeader(std::string path, std::string content_type) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::vector<char> result;
    std::vector<char> vec_header = getHeader(file.tellg(), content_type);
    std::vector<char> vec_html;

    if (!file.eof() && !file.fail()) {
        file.seekg(0, std::ios_base::end);
        std::streampos fileSize = file.tellg();
        vec_html.resize(fileSize);

        file.seekg(0, std::ios_base::beg);
        file.read(&vec_html[0], fileSize);
    }

    result.insert( result.begin(), vec_header.begin(), vec_header.end() );
    result.insert( result.end(), vec_html.begin(), vec_html.end() );


    return result;
}

std::vector<std::string> addresses;

int main() {
    std::cout << "hello" << std::endl;

    // todo change if unix env (#DEFINE ?)
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
            if(addresses.size() > 50) {
                // todo add timer per address for anti spam
                std::cout << addresses.size() << std::endl;
            } else {

                int received = recv(client, buf, 1024, 0);
                std::vector<char> data(buf, buf + 1024);
                char *html_needed = NULL;
                char *css_needed = NULL;
                char *img_needed = NULL;
                html_needed = strstr (buf, "text/html");
                css_needed = strstr (buf, "text/css");
                img_needed = strstr (buf, "img.png");
                // todo use GET ./site.css HTTP/1.1 instead
                if(html_needed) {
                    std::vector<char> result = getDataWithHeader("site.html", "text/html");
                    char* bytes = &result[0];
                    send(client, bytes, strlen(bytes)-1, 0);
                    std::cout << "sent" << std::endl;
                } else if(css_needed) {
                    std::vector<char> result = getDataWithHeader("site.css", "text/css");
                    char* bytes = &result[0];
                    for(int i=0 ; i<10 ; ++i)
                    {
                    std::cout << bytes[i];
                    }
                    send(client, bytes, strlen(bytes)-1, 0);
                    std::cout << "sent css" << std::endl;
                }
                //} else if(std::find(buf.begin(), buf.end(), "image/avif")) {
                    // read css + send
                //} else {
                    // std::cout << "Request not understood" << std::endl;
                //}
                
                // if Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
                // elif Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
                
                addresses.push_back(inet_ntoa(client_addr.sin_addr));

                // todo append header + current response
                //char* bytes = getHTML();
                //send(client, bytes, strlen(bytes)-1, 0);
                //send(client, response, strlen(response)-1, 0);
            }
        }

		const auto last_error = WSAGetLastError();
		
		if(last_error > 0)
		{
			std::cout << "Error: " << last_error << std::endl;
		}

std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        closesocket(client);
        std::cout << "socket closed" << std::endl;
    }
    
}