#include "util.hh"
#include <asm-generic/socket.h>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static int32_t query(int fd, const char *text) {
  uint32_t len = (uint32_t)strlen(text);

  if (len > k_max_msg) {
    return -1;
  }

  char wbuf[4 + k_max_msg];

  memcpy(wbuf, &len, 4); // assume little endian
  memcpy(&wbuf[4], text, len);

  if (int32_t err = write_all(fd, wbuf, 4 + len)) {
    return err;
  }

  // 4 bytes header
  char rbuf[4 + k_max_msg + 1];

  errno = 0;
  int32_t err = read_full(fd, rbuf, 4);

  if (err) {
    if (errno == 0) {
      msg("EOF");
    } else {
      msg("read() erro");
    }

    return err;
  }

  memcpy(&len, rbuf, 4);

  if (len > k_max_msg) {
    msg("Too long");
    return err;
  }

  // do something
  //
  rbuf[4 + len] = '\0';
  std::cout << "server says: " << &rbuf[4] << std::endl;
  return 0;
}

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

  // multiple requests
  int32_t err = query(fd, "hello1");

  if (err) {
    goto L_DONE;
  }
  err = query(fd, "hello2");

  if (err)
    goto L_DONE;

  err = query(fd, "hello3");

  if (err)
    goto L_DONE;

L_DONE:
  close(fd);
  return 0;
}
