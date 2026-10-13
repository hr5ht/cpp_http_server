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
    pause();  // block until a signal; accept() comes next

    close(listen_fd);
    return 0;
}
