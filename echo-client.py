import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 9999  # The port used by the server

str_0 = b"ZehnByteeeZehnByteeeZehnByteeeZehnByteeeZehnByteee"
str_1 = b"ZehnByteeeZehnByteeeZehnByteeeZehnByteeeZehnByteeeZehn"
 
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    # 01010100
    s.send(str_0)
    s.send(str_1)
    s.send(str_0)
    s.send(str_1)
    s.send(str_0)
    s.send(str_1)
    s.send(str_0)
    s.send(str_0)
    data = s.recv(1024)

print(f"Received {data!r}")