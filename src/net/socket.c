#include "socket.h"
#include "logging.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define NB_AVAILABLE_BUFFERS 5

static socket_buf_t buffers[NB_AVAILABLE_BUFFERS] = { 0 };
static uint8_t next_available_buffer = 0;

void socket_bufferize(int sockfd, uint8_t* data, uint16_t len)
{
  socket_buf_t* buf = NULL;

  for (uint8_t i = 0; i < NB_AVAILABLE_BUFFERS; i++) {
    if (buffers[i].sockfd == sockfd) {
      buf = &buffers[i];
      break;
    }
  }

  if (buf == NULL && next_available_buffer < NB_AVAILABLE_BUFFERS) {
    buf = &buffers[next_available_buffer++];
    memset(buf, 0, sizeof(socket_buf_t));
    buf->sockfd = sockfd;
    NET_DEBUG("created socket buffer for sockfd=%d", sockfd);
  }

  if (buf == NULL) {
    NET_DEBUG("could not create a socket buffer for sockfd=%d", sockfd);
    return;
  }

  for (uint16_t i = 0; i < len; i++) {
    if (buf->len == BUFFER_LEN) {
      NET_DEBUG("socket buffer for sockfd=%d is full", sockfd);
      break;
    }

    buf->buffer[buf->write++] = data[i];
    buf->len++;

    if (buf->write == BUFFER_LEN) {
      buf->write = 0;
    }
  }

  NET_DEBUG(
    "added data to socket buffer for sockfd=%d: len=%d read=%d write=%d",
    len,
    sockfd,
    buf->len,
    buf->read,
    buf->write);
}

ssize_t socket_unbufferize(int sockfd, uint8_t* buf, size_t len)
{
  for (uint8_t i = 0; i < NB_AVAILABLE_BUFFERS; i++) {
    if (buffers[i].sockfd == sockfd) {
      while (buffers[i].len < 1) {
        ;
      }

      NET_DEBUG("found socket buffer for sockfd=%d: len=%d read=%d write=%d",
                sockfd,
                buffers[i].len,
                buffers[i].read,
                buffers[i].write);

      for (size_t j = 0; j < len; j++) {
        if (buffers[i].len == 0) {
          NET_DEBUG("buffer for sockfd=%d is empty", sockfd);
          return j;
        }

        buf[j] = buffers[i].buffer[buffers[i].read++];
        buffers[i].len--;

        if (buffers[i].read == BUFFER_LEN) {
          buffers[i].read = 0;
        }
      }

      NET_DEBUG("unbufferized %lld bytes", len);

      return len;
    }
  }

  NET_DEBUG("socket buffer for sockfd=%d not found", sockfd);

  return -1;
}

int socket_delete_buffer(int sockfd)
{
  for (uint8_t i = 0; i < NB_AVAILABLE_BUFFERS; i++) {
    if (buffers[i].sockfd == sockfd) {
      memset(&buffers[i], 0, sizeof(socket_buf_t));

      return 0;
    }
  }

  return -1;
}

bool is_protocol_supported(int type, int protocol)
{
  switch (type) {
    case SOCK_DGRAM:
      switch (protocol) {
        case IPPROTO_UDP:
          return true;
      }
      break;
  }

  return false;
}
