import socket

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 9999  # Port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    print(f'Listening for connections on {HOST}:{PORT}...')
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        binary_string = ""
        i = 0
        while True:
            data = conn.recv(1024)
            print("Received Package ", i)
            i += 1
            if len(data) == 50:
                binary_string += '0'
            elif len(data) == 60:
                binary_string += '1'
            if not data:
                print("Binary string received: ", binary_string)
                # 0 and 1, with base of 2
                binary_int = int(binary_string, 2);
                
                # Getting the byte number
                byte_number = binary_int.bit_length() + 7 // 8
                
                # Getting an array of bytes
                binary_array = binary_int.to_bytes(byte_number, "big")
                
                # Converting the array into ASCII text
                ascii_text = binary_array.decode()
                
                # Getting the ASCII value
                print("Message received: ", ascii_text)

                # Close the connection with the client
                conn.close()

                # Breaking once connection closed
                break