#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;  // POSIX calls that clash with std names (bind) need :: prefix

namespace {

constexpr int kPort = 8080;
constexpr int kBacklog = 128;  // macOS clamps this to kern.ipc.somaxconn

[[noreturn]] void die(const char* what) {
    perror(what);
    exit(EXIT_FAILURE);
}

}  // namespace

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) die("socket");

    // Let bind() succeed while old connections on this port sit in TIME_WAIT.
    int yes = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) die("setsockopt");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(kPort);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (::bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) die("bind");
    if (listen(listen_fd, kBacklog) < 0) die("listen");

    cout << "listening on 127.0.0.1:" << kPort << endl;

    sockaddr_in client{};
    socklen_t client_len = sizeof(client);
    int client_fd = accept(listen_fd, reinterpret_cast<sockaddr*>(&client), &client_len);
    if (client_fd < 0) die("accept");

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
    cout << "client connected: " << ip << ':' << ntohs(client.sin_port) << endl;

    char buf[4096];
    ssize_t n;
    while ((n = recv(client_fd, buf, sizeof(buf), 0)) > 0) {
        // send() may write fewer than n bytes; handled properly on Day 4
        if (send(client_fd, buf, static_cast<size_t>(n), 0) < 0) die("send");
    }
    if (n < 0) die("recv");
    cout << "client disconnected" << endl;

    close(client_fd);
    close(listen_fd);
    return 0;
}
