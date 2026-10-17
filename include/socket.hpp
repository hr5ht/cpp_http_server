#pragma once

class Socket {
public:
    Socket() = default;
    explicit Socket(int fd) : fd_(fd) {}
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    int fd() const { return fd_; }

private:
    int fd_ = -1;
};
