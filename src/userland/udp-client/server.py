import socket


PORT = 22000
BUFSIZE = 1024

server = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
server.bind(("127.0.0.1", PORT))

print("UDP server up and listening")

while True:
    try:
        (message, address) = server.recvfrom(BUFSIZE)
        print("Client said: {}".format(message.decode()))

        server.sendto("Hello from the host!".encode(), address)
        print("Message sent!")
    except KeyboardInterrupt:
        print("\b\bExiting...")
        break

server.close()
