#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace {

constexpr int kPort = 8080;
constexpr int kBacklog = 128;  // macOS clamps this to kern.ipc.somaxconn

[[noreturn]] void die(const char* what) {
    std::perror(what);
    std::exit(EXIT_FAILURE);
}

}  // namespace

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) die("socket");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(kPort);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) die("bind");
    if (listen(listen_fd, kBacklog) < 0) die("listen");

    std::cout << "listening on 127.0.0.1:" << kPort << std::endl;

    sockaddr_in client{};
    socklen_t client_len = sizeof(client);
    int client_fd = accept(listen_fd, reinterpret_cast<sockaddr*>(&client), &client_len);
    if (client_fd < 0) die("accept");

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
    std::cout << "client connected: " << ip << ':' << ntohs(client.sin_port) << std::endl;

    char buf[4096];
    ssize_t n;
    while ((n = recv(client_fd, buf, sizeof(buf), 0)) > 0) {
        // send() may write fewer than n bytes; handled properly on Day 4
        if (send(client_fd, buf, static_cast<size_t>(n), 0) < 0) die("send");
    }
    if (n < 0) die("recv");
    std::cout << "client disconnected" << std::endl;

    close(client_fd);
    close(listen_fd);
    return 0;
}
