# Demo 1: Inter packages time:
- Client: ./client -D 127.0.0.1 -P 9999 -t `./iat_encode text.txt 200 100`
- Server: ./own_server -P 9999 200 100

# Demo 2: Sequence Ordering packages:
- Client: ./client -P 9999 -D 127.0.0.1 -s `./seq_encode text.txt 256 2`
- Server: ./server -P 9999