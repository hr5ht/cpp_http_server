#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;  // POSIX calls that clash with std names (bind) need :: prefix

namespace {

constexpr int PORT = 8080;
constexpr int BACKLOG = 128;  // macOS clamps this to kern.ipc.somaxconn

[[noreturn]] void die(const char* what) {
    perror(what);
    exit(EXIT_FAILURE);
}

}  // namespace

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) die("socket");

    // Let bind() succeed while old connections on this port sit in TIME_WAIT.
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) die("setsockopt");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (::bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0)
        die("bind");
    if (listen(server_fd, BACKLOG) < 0) die("listen");

    cout << "listening on 127.0.0.1:" << PORT << endl;

    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
    if (client_fd < 0) die("accept");

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
    cout << "client connected: " << ip << ':' << ntohs(client_addr.sin_port) << endl;

    char buffer[4096];
    ssize_t bytes;
    while ((bytes = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        // send() may write fewer than `bytes`; handled properly on Day 4
        if (send(client_fd, buffer, static_cast<size_t>(bytes), 0) < 0) die("send");
    }
    if (bytes < 0) die("recv");
    cout << "client disconnected" << endl;

    close(client_fd);
    close(server_fd);
    return 0;
}
