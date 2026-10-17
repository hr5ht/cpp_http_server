#include "socket.hpp"

#include <unistd.h>

#include <utility>

using namespace std;

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
