#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT    22000
#define BUFSIZE 1024

#ifdef __willos__
// In QEMU, the gateway will forward packets to the host.
#define SERVER_IP "10.0.2.2"
#else
#define SERVER_IP "127.0.0.1"
#endif

int main(void)
{
  const char* message = "Bonjour depuis le client !";
  char buf[BUFSIZE] = { 0 };

  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sockfd < 0) {
    printf("Error while creating socket\n");
    return -1;
  }

  printf("Socket created successfully\n");

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

  socklen_t server_addr_len = sizeof(struct sockaddr_in);

  if (sendto(sockfd,
             message,
             strlen(message),
             0,
             (struct sockaddr*)&server_addr,
             server_addr_len) < 0) {
    printf("Failed to send message\n");
    return -1;
  }
  printf("Message sent!\n");

  int bytes_received = recvfrom(sockfd,
                                buf,
                                sizeof(buf),
                                0,
                                (struct sockaddr*)&server_addr,
                                &server_addr_len);
  if (bytes_received < 0) {
    printf("Error while receiving server's message\n");
    return -1;
  }
  printf("Server said: %s (%d)\n", buf, bytes_received);

  close(sockfd);

  return 0;
}
