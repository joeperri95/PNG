# Inflate 

Implementations of the decoding portions of RFC 1950, 1951, 1952, and 2083

This project was originally only a PNG decoder.
Turns out in building a PNG decoder you get a free gunzip implementation since gzip is just a
deflate stream with a small header.

To do functionality
* Viewer application
* Adam7 deinterlacing
* Ancillary chunks
* Pallete chunks
* More unit tests / CI
* Auto allocate memory as needed for inflate
* Arg parsing

Possibly someday a compressor.
