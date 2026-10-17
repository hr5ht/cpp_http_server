#pragma once

#include <netinet/in.h>

#include <cstddef>

class Socket {
public:
    Socket() = default;
    explicit Socket(int fd) : fd_(fd) {}
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    static Socket create();

    void set_reuse_addr();
    void bind(int port);
    void listen(int backlog);
    Socket accept(sockaddr_in& client_addr);

    size_t recv(char* buffer, size_t size);
    size_t send(const char* data, size_t size);

    int fd() const { return fd_; }

private:
    int fd_ = -1;
};
