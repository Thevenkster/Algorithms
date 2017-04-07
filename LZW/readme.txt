LZW Project README: Adam Browne (author)

The purpose of this project was to implement the LZW Compression / Decompression
Algorithms / tweak and configure them so they will encode files [9,16] bits.


./lzw435 <c/e> <file>.ext.<lzw>
Compresses and expands the file.

Part 1:
I had to keep track of the padding and limit the size of the dictionary so it would stop recording after the dictionary exceeded 512. Once this was done, you could extract the bit strings by knowing their bit lengths.

Part 2:
By modifying the compress and decompression algorithms so they return strings and handle the bit sizes 9 - 16 (also changing the function signature of int2BinaryString & more) I was able to have the words encoded 2^9 - 2^16

