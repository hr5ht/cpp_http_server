# http-server-cpp

A multithreaded HTTP/1.1 server written in C++17 directly on POSIX TCP sockets:
bounded worker thread pool, keep-alive, timeouts, graceful shutdown, and real
benchmark numbers.

## Build

```sh
cmake -B build
cmake --build build
./build/server
```

Debug build with AddressSanitizer + UndefinedBehaviorSanitizer:

```sh
cmake --preset debug
cmake --build --preset debug
./build-debug/server
```
