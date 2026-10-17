#include <arpa/inet.h>

#include <exception>
#include <iostream>

#include "socket.hpp"

using namespace std;

constexpr int PORT = 8080;
constexpr int BACKLOG = 128;

int main() {
    try {
        Socket server = Socket::create();
        server.set_reuse_addr();
        server.bind(PORT);
        server.listen(BACKLOG);

        cout << "listening on 127.0.0.1:" << PORT << endl;

        sockaddr_in client_addr{};
        Socket client = server.accept(client_addr);

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        cout << "client connected: " << ip << ':' << ntohs(client_addr.sin_port) << endl;

        char buffer[4096];
        size_t bytes;
        while ((bytes = client.recv(buffer, sizeof(buffer))) > 0) {
            client.send(buffer, bytes);
        }
        cout << "client disconnected" << endl;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
