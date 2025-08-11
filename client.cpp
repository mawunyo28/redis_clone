#include "util.hh"
#include <asm-generic/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd < 0) {
    die("socket()");
  }

  sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1 (localhost)

  int rv = connect(fd, (const sockaddr *)&addr, sizeof(addr));

  if (rv) {
    die("connect");
  }

  char msg[] = "hello";
  write(fd, msg, strlen(msg));

  char rbuf[64] = {};
  ssize_t n =
      read(fd, rbuf, sizeof(rbuf) - 1); // Space for terminating character

  if (n < 0) {
    die("read");
  }

  std::cout << "Server says: " << rbuf << std::endl;
  close(fd);

  return 0;
}
