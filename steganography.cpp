#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// 1. Data structure to hold PPM image data
struct PPMImage {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};

// 2. Read PPM image from disk in binary mode
bool loadPPM(const std::string& filename, PPMImage& img) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file -> " << filename << "\n";
        return false;
    }

    std::string format;
    int maxVal;

    // Read header information
    if (!(file >> format >> img.width >> img.height >> maxVal)) {
        std::cerr << "Error: Failed to read PPM header.\n";
        return false;
    }

    if (format != "P6" || maxVal != 255) {
        std::cerr << "Error: Only P6 format with max value 255 is supported.\n";
        return false;
    }

    file.get(); // Skip the single whitespace/newline byte after header

    // Calculate total byte size (width * height * 3 color channels)
    size_t totalBytes = static_cast<size_t>(img.width) * img.height * 3;
    img.pixels.resize(totalBytes);

    // Read raw pixel data into memory
    file.read(reinterpret_cast<char*>(img.pixels.data()), totalBytes);

    return file.good();
}

// 3. Save PPM image to disk in binary mode
bool savePPM(const std::string& filename, const PPMImage& img) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file -> " << filename << "\n";
        return false;
    }

    // Write header
    file << "P6\n" << img.width << " " << img.height << "\n255\n";

    // Write pixel buffer directly to disk
    size_t totalBytes = img.pixels.size();
    file.write(reinterpret_cast<const char*>(img.pixels.data()), totalBytes);

    return file.good();
}

// 4. Embed secret text into the least significant bit (LSB) of pixels
bool embedMessage(PPMImage& img, const std::string& message) {
    // Capacity check: (message length + null terminator \0) * 8 bits
    size_t requiredBytes = (message.size() + 1) * 8;

    if (img.pixels.size() < requiredBytes) {
        std::cerr << "Error: Image capacity is too small for this message!\n";
        return false;
    }

    size_t pixelIndex = 0;
    std::string secretPayload = message + '\0'; // Null terminator to mark end

    for (char c : secretPayload) {
        // Extract 8 bits from MSB to LSB
        for (int i = 7; i >= 0; --i) {
            unsigned char bit = (static_cast<unsigned char>(c) >> i) & 1;

            // Clear the lowest bit (& 0xFE) and inject secret bit (| bit)
            img.pixels[pixelIndex] = (img.pixels[pixelIndex] & 0xFE) | bit;

            pixelIndex++;
        }
    }

    return true;
}

// 5. Extract secret text from pixel LSBs
std::string extractMessage(const PPMImage& img) {
    std::string secretMessage = "";
    char character = 0;
    int bitCounter = 0;

    for (unsigned char byte : img.pixels) {
        unsigned char bit = byte & 1; // Extract the least significant bit

        // Reconstruct character by shifting bits left
        character = (character << 1) | bit;
        bitCounter++;

        // 8 bits form 1 character
        if (bitCounter == 8) {
            if (character == '\0') {
                break; // End of message reached
            }
            secretMessage += character;
            character = 0;
            bitCounter = 0;
        }
    }

    return secretMessage;
}

// 6. Command-Line Interface (CLI) Entry Point
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage:\n";
        std::cout << "  Embed: " << argv[0] << " embed <input.ppm> <output.ppm> \"Your message\"\n";
        std::cout << "  Extract: " << argv[0] << " extract <image.ppm>\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "embed") {
        if (argc < 5) {
            std::cerr << "Missing parameters!\n";
            std::cerr << "Usage: " << argv[0] << " embed <input.ppm> <output.ppm> \"Your message\"\n";
            return 1;
        }

        std::string inputPath = argv[2];
        std::string outputPath = argv[3];
        std::string message = argv[4];

        PPMImage image;
        if (!loadPPM(inputPath, image)) return 1;
        if (!embedMessage(image, message)) return 1;
        if (!savePPM(outputPath, image)) return 1;

        std::cout << "[+] Message successfully embedded -> " << outputPath << "\n";

    } else if (mode == "extract") {
        if (argc < 3) {
            std::cerr << "Missing parameters!\n";
            std::cerr << "Usage: " << argv[0] << " extract <image.ppm>\n";
            return 1;
        }

        std::string inputPath = argv[2];
        PPMImage image;
        if (!loadPPM(inputPath, image)) return 1;

        std::string message = extractMessage(image);
        std::cout << "------------------------------------\n";
        std::cout << "Extracted Message: \n\"" << message << "\"\n";
        std::cout << "------------------------------------\n";

    } else {
        std::cerr << "Invalid mode! Use 'embed' or 'extract'.\n";
        return 1;
    }

    return 0;
}




