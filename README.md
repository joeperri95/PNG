# Inflate 

Implementations of the decoding portions of RFC 1950, 1951, 1952, and 2083

This project was originally a no dependency PNG decoder only.
Turns out in building a PNG decoder you get a free gunzip implementation since gzip is just a zlib
stream with a small header.

To do functionality
* Ancillary chunks
* Pallete chunks
* More unit tests / CI
* Auto allocate memory as needed for inflate


Possibly someday a compressor.
