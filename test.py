binary_string = "01110100011010001111010001101110011001110010000010001000101101001111101100011101110000"
for i in range(0, len(binary_string), 7):
    print(binary_string[i:i+8])
binary_int = int(binary_string, 2)

print(binary_int)
                
# Getting the byte number
byte_number = binary_int.bit_length() + 7 // 8
print(byte_number)

# Getting an array of bytes
binary_array = binary_int.to_bytes(byte_number, "big")
print(binary_array)

# # Converting the array into ASCII text
# ascii_text = binary_array.encode("utf-8").decode("utf-8")

# # Getting the ASCII value
# print("Message received: ", ascii_text)