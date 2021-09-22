# Inflate 

Implementations of the decoding portions of RFC 1950, 1951, 1952, and 2083

This project was originally only a PNG decoder.
Turns out in building a PNG decoder you get a free gunzip implementation since gzip is just a
deflate stream with a small header.

## To do functionality to become compliant
* Adam7 deinterlacing
* Sub byte grayscale pixels
* Ancillary chunks
* Pallete chunks

## Nice to haves
* Viewer application
* More unit tests / CI
* Application tests
* Auto allocate memory as needed for inflate
* Arg parsing

Possibly someday a compressor.
