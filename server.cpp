#include <iostream>
#include <winsock2.h>
#include <future>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <regex>
#include <map>
#include <thread>

const char* file_visitors = "list_visitors.lst";
const char* file_logs = "logs.lst";
const std::map<std::string, std::string> types_map = {
    { "html", "text/html"},
    { "css", "text/css"},
    { "png", "image/png"},
    { "ico", "image/ico"},
    { "txt", "text/txt"}
};
const int BUFFER_REQUEST_SIZE = 1024;
std::map<std::string, int> addresses;
time_t last_time;
enum class ServiceType { PROXY, LOADBALANCER, SERVER };
ServiceType service_type;

std::string getBestTypeContent(std::string type) {
    if (types_map.count(type)) {
        return types_map.at(type);
    }
    return {};
}

std::vector<char> getHeader(int code, int size, std::string content_type) {
    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    std::string s = "HTTP/1.1 " + std::to_string(code) + "\r\n"
"Content-Length: " + std::to_string(size) + "\r\n" 
"Connection: close\r\n"
"Date: " + buf + "\r\n"
"Content-type: " + content_type + "\r\n"
"\r\n";
    std::vector<char> vec(s.begin(), s.end());
    return vec;
}

void logVisitor(std::string addr) {
    std::ifstream stream_visitors(file_visitors);
    std::ofstream updated("tmp_" + std::string(file_visitors));
    if (!stream_visitors) {
        std::cerr << "file open failed: " << std::strerror(errno) << "\n";
    }
    std::string visitor;
    int visits;
    bool found = false;
    while (stream_visitors >> visitor >> visits) {
        if (visitor == addr) {
            updated << visitor << " " << visits+1 << std::endl;
            found = true;
            break;
        } else {
            updated << visitor << " " << visits << std::endl;
        }
    }
    if (!found) {
        updated << addr << " " << 1 << std::endl;
    }

    stream_visitors.close();
    updated.close();

    remove(file_visitors);
    rename(("tmp_" + std::string(file_visitors)).c_str(), file_visitors);
}

std::vector<char> getDataWithHeader(int code, std::string path, std::string content_type) {
    size_t pos = path.find("%20");
    while(pos != std::string::npos) {
        path.replace(pos, 3, " ");
        pos = path.find("%20", pos + 1);
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (file.fail()) {
        return {};
    }

    std::vector<char> vec_header = getHeader(code, file.tellg(), content_type);
    std::vector<char> vec_data;
    if (!file.eof() && !file.fail()) {
        file.seekg(0, std::ios_base::end);
        std::streampos fileSize = file.tellg();
        vec_data.resize(fileSize);

        file.seekg(0, std::ios_base::beg);
        file.read(&vec_data[0], fileSize);
    }
    std::vector<char> result;
    result.insert( result.begin(), vec_header.begin(), vec_header.end() );
    result.insert( result.end(), vec_data.begin(), vec_data.end() );

    return result;
}

void handleClient(SOCKET client, std::string path, std::string addr) {
    
    time_t current_time  = std::time(0);
    if (current_time - last_time >= 60) {
        addresses.clear();
        last_time = current_time;
    }
    if (addresses.count(addr)) {
        if(addresses[addr] >= 30) {
            // todo send 429 ?
            return;
        } else {
            addresses[addr] += 1;
        }
    } else {
        addresses[addr] = 1;
    }

    char buf[BUFFER_REQUEST_SIZE] = { 0 };
    int received = recv(client, buf, BUFFER_REQUEST_SIZE, 0);
    // todo generator html code
    if(received >= BUFFER_REQUEST_SIZE) {
        std::vector<char> result = getDataWithHeader(401, "401.html", "text/html");
        send(client, &result[0], result.size(), 0);
        return;
    } else if(received <= 0) {
        std::vector<char> result = getDataWithHeader(400, "400.html", "text/html");
        send(client, &result[0], result.size(), 0);
        return;
    }
    std::string data(buf, buf + BUFFER_REQUEST_SIZE);

    if(service_type == ServiceType::PROXY) {
        SOCKADDR_IN address, client_addr;
        address.sin_addr.s_addr = inet_addr("127.0.0.2");
        address.sin_family = AF_INET;
        address.sin_port = htons(65432);
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        int client_fd = connect(sock, (struct sockaddr*)&address,sizeof(address));
        send(sock, data.c_str(), data.size(), 0);
        char buffer[1024] = { 0 };
        int valread = recv(sock, buffer, BUFFER_REQUEST_SIZE, 0);
        printf("%s\n", buffer);
        closesocket(client_fd);
        
        send(client, &buffer[0], BUFFER_REQUEST_SIZE, 0);
        return;
    } else if(service_type == ServiceType::SERVER) {
        // todo POST
        const std::regex rgx("GET (/[^ ]*)");
        const std::regex rgx_type("\\.([^ ]*)");
        std::smatch matches, match_type;
        if(std::regex_search(data, matches, rgx)) {
            std::string type_requested(matches[1]), file_requested;
            if(!std::regex_search(type_requested, match_type, rgx_type)) {
                type_requested = "html";
                file_requested = "." + path + std::string(matches[1]) + ".html";
            } else {
                type_requested = match_type[1];
                file_requested = "." + path + std::string(matches[1]);
            }
            if(type_requested == "html") {
                logVisitor(addr);
            }
            std::vector<char> result = getDataWithHeader(200, file_requested, getBestTypeContent(type_requested));
            if (result.empty()) {
                // todo refactor not to call getDataWithHeader 2x
                std::vector<char> result = getDataWithHeader(404, "404.html", "text/html");
                send(client, &result[0], result.size(), 0);
            } else {
                send(client, &result[0], result.size(), 0);
            }
        } else {
            // bad request
            std::vector<char> result = getDataWithHeader(401, "401.html", "text/html");
            send(client, &result[0], result.size(), 0);
        }
    }
}

int main(int argc, const char* argv[]) {
    // todo log cerr with [ERROR] tag (+ async)

    std::string path;
    if (argc != 2) {
        std::cout << "Usage: ./server <path> <service_type>" << std::endl;
        std::cout << "  <service_type> = ['Server' | 'Proxy' | 'LoadBalancer']" << std::endl;
        return 1;
    } else {
        path = argv[1];
        service_type = ServiceType::SERVER;
    }
    std::cout << "Starting server for " << path << std::endl;

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
    std::cout << "Started server for " << path << std::endl;
    freopen( file_logs, "a", stdout );
    std::cout << "Started server for " << path << std::endl;

    int client_addr_size = sizeof(client_addr);
    for(;;) {
		SOCKET client;

		if ((client = accept(server, reinterpret_cast<SOCKADDR *>(&client_addr), &client_addr_size)) != INVALID_SOCKET)
		{
            std::cout << "client accepted:" << inet_ntoa(client_addr.sin_addr) << std::endl;
        
            // todo thread pool
            std::thread client_thread(handleClient, client, path, inet_ntoa(client_addr.sin_addr));
            client_thread.detach();
        }

		const auto last_error = WSAGetLastError();
		if(last_error > 0) {
			std::cout << "Error: " << last_error << std::endl;
		}
    }
    
    return 0;
}