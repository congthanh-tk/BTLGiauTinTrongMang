import socket
from time import sleep

str_0 = b"ZehnByteeeZehnByteeeZehnByteeeZehnByteeeZehnByteee"+b" "*50
str_1 = b"ZehnByteeeZehnByteeeZehnByteeeZehnByteeeZehnByteeeZehnByteee"+b" "*40

msg = input("Nhập thông điệp cần gửi: ")
HOST = input("Nhập địa chỉ đích: ")
PORT = int(input("Nhập cổng đích: "))
bin_msg = ''.join('{:08b}'.format(b) for b in msg.encode('utf8'))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    for i in bin_msg:
        print("Send package: ", i)
        if i == "0":
            s.send(str_0)
        elif i == "1":
            s.send(str_1)
        sleep(0.5)

    print("Sent message success!")
    # s.close()