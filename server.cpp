#include "util.hh"
#include <asm-generic/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static void do_something(int connfd) {
  char rbuf[64] = {};

  ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);

  if (n < 0) {
    msg("read() error");
    return;
  }

  std::cout << "client says " << rbuf << std::endl;

  char wbuf[] = "world";
  write(connfd, wbuf, strlen(wbuf));
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

    do_something(connfd);

    close(connfd);
  }
  return 0;
}
