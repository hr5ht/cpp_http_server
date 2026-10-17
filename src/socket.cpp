#include "socket.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>
#include <utility>

using namespace std;

namespace {

template <typename T>
T check(T result, const char* what) {
    if (result < 0) throw system_error(errno, generic_category(), what);
    return result;
}

}  // namespace

Socket::~Socket() {
    if (fd_ >= 0) ::close(fd_);
}

Socket::Socket(Socket&& other) noexcept : fd_(exchange(other.fd_, -1)) {}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        if (fd_ >= 0) ::close(fd_);
        fd_ = exchange(other.fd_, -1);
    }
    return *this;
}

Socket Socket::create() { return Socket(check(::socket(AF_INET, SOCK_STREAM, 0), "socket")); }

void Socket::set_reuse_addr() {
    int opt = 1;
    check(::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "setsockopt");
}

void Socket::bind(int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    check(::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)), "bind");
}

void Socket::listen(int backlog) { check(::listen(fd_, backlog), "listen"); }

Socket Socket::accept(sockaddr_in& client_addr) {
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
    return Socket(check(client_fd, "accept"));
}

size_t Socket::recv(char* buffer, size_t size) {
    return static_cast<size_t>(check(::recv(fd_, buffer, size, 0), "recv"));
}

size_t Socket::send(const char* data, size_t size) {
    return static_cast<size_t>(check(::send(fd_, data, size, 0), "send"));
}
