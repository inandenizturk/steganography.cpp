# PPM Image Steganography (LSB)

A lightweight, dependency-free C++ CLI tool that embeds and extracts secret text messages inside PPM (P6) binary images using the Least Significant Bit (LSB) technique.

## Features

- **Zero External Dependencies:** Built entirely with modern C++ standard libraries (`<fstream>`, `<vector>`, `<string>`).
- **Raw Binary Processing:** Direct parsing and manipulation of PPM (P6 format) image byte buffers.
- **Bitwise Steganography:** Injects text bit-by-bit into the least significant bit of color channels.
- **Portable CLI:** Clean command-line interface controlled via standard execution arguments.

---

## Build

Compile the project using any C++17 compatible compiler:

```bash
g++ -std=c++17 -O2 stego.cpp -o steg

# 1. Derleme
g++ -std=c++17 -O2 stego.cpp -o stego

# 2. Test resmi üretme (input.ppm)
printf "P6\n10 10\n255\n" > input.ppm && head -c 300 /dev/zero | tr '\0' '\377' >> input.ppm

# 3. Mesaj gömme (Embed)
./stego embed input.ppm output.ppm "Secret Message"

# 4. Mesajı çözme (Extract)
./stego extract output.ppm
