#include "util.hh"
#include <asm-generic/socket.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
static int32_t one_request(int connfd) {
  // 4 bytes header
  //
  char rbuf[4 + k_max_msg + 1];
  errno = 0;

  int32_t err = read_full(connfd, rbuf, 4);

  if (err) {
    if (errno == 0) {
      msg("EOF");
    } else {
      msg("read() error");
    }

    return err;
  }

  uint32_t len = 0;
  memcpy(&len, rbuf, 4); // assume little endian
  if (len > k_max_msg) {
    msg("too long");
    return -1;
  }

  // do something
  //
  rbuf[4 + len] = '\0';
  std::cout << "Client says: " << &rbuf[4] << std::endl;

  // reply using the same protocol
  const char reply[] = "world";
  char wbuf[4 + sizeof(reply)];
  len = (uint32_t)strlen(reply);
  memcpy(wbuf, &len, 4);
  memcpy(&wbuf[4], reply, len);

  return write_all(connfd, wbuf, 4 + len);
}

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  sockaddr_in addr = {};

  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(0); // 0.0.0.0:1234

  int rv =
      bind(fd, (const sockaddr *)&addr, sizeof(addr)); // binding socket to fd

  if (rv) {
    die("bind()");
  }

  rv = listen(fd, SOMAXCONN);

  if (rv) {
    die("listen()");
  }

  while (true) {
    // accept

    sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);

    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);

    if (connfd < 0) {
      continue; // error Todo: Solve
    }

    // only serves one client connection at once
    while (true) {
      int32_t err = one_request(connfd);

      if (err) {
        break;
      }
    }

    close(connfd);
  }
  return 0;
}
