#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <algorithm>

using namespace std;

const int FONT_WIDTH = 5;
const int FONT_HEIGHT = 7;
unordered_map<char, vector<string>> fontMap = {
    {'A', {"  #  ", " # # ", "#####", "#   #", "#   #", "     ", "     "}},
    {'B', {"#### ", "#   #", "#### ", "#   #", "#### ", "     ", "     "}},
    {'C', {" ####", "#    ", "#    ", "#    ", " ####", "     ", "     "}},
    {'D', {"###  ", "#  # ", "#   #", "#  # ", "###  ", "     ", "     "}},
    {'E', {"#####", "#    ", "#### ", "#    ", "#####", "     ", "     "}},
    {'0', {" ### ", "#   #", "# # #", "#   #", " ### ", "     ", "     "}},
    {'1', {"  #  ", " ##  ", "  #  ", "  #  ", " ### ", "     ", "     "}},
    {'2', {" ### ", "#   #", "  ## ", " #   ", "#####", "     ", "     "}},
    {'3', {" ### ", "#   #", "  ## ", "#   #", " ### ", "     ", "     "}},
};

struct BitmapImage {
    int width, height;
    vector<unsigned char> pixels;

    BitmapImage(int w, int h) : width(w), height(h), pixels(w * h, 255) {}

    void setPixel(int x, int y, unsigned char value) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * width + x] = value;
        }
    }

    void saveToFile(const string& filename) {
        ofstream file(filename, ios::binary);
        file << "P5\n" << width << " " << height << "\n255\n";
        file.write(reinterpret_cast<char*>(pixels.data()), pixels.size());
        file.close();
    }
};

BitmapImage renderTextToBitmap(const string& text, int fontSize, int& outWidth, int& outHeight) {
    fontSize = max(5, fontSize);
    int width = text.length() * FONT_WIDTH * fontSize / 5;
    int height = FONT_HEIGHT * fontSize / 7;
    BitmapImage bitmap(width, height);

    for (size_t i = 0; i < text.length(); ++i) {
        char c = toupper(text[i]);
        if (fontMap.find(c) == fontMap.end()) continue;
        int xOffset = i * FONT_WIDTH * fontSize / 5;

        for (int y = 0; y < FONT_HEIGHT; ++y) {
            for (int x = 0; x < FONT_WIDTH; ++x) {
                if (fontMap[c][y][x] == '#') {
                    for (int dy = 0; dy < max(1, fontSize / 7); ++dy) {
                        for (int dx = 0; dx < max(1, fontSize / 5); ++dx) {
                            bitmap.setPixel(xOffset + x * (fontSize / 5) + dx, y * (fontSize / 7) + dy, 0);
                        }
                    }
                }
            }
        }
    }

    outWidth = width;
    outHeight = height;
    return bitmap;
}

BitmapImage resizeBitmap(const BitmapImage& input, int newWidth, int newHeight) {
    BitmapImage resized(newWidth, newHeight);
    float xRatio = static_cast<float>(input.width) / newWidth;
    float yRatio = static_cast<float>(input.height) / newHeight;

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int srcX = round(x * xRatio);
            int srcY = round(y * yRatio);
            resized.setPixel(x, y, input.pixels[min(srcY, input.height - 1) * input.width + min(srcX, input.width - 1)]);
        }
    }

    return resized;
}

string sanitizeFilename(const string& text) {
    string filename = text;
    replace_if(filename.begin(), filename.end(), [](char c) { return !isalnum(c); }, '_');
    return filename;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " <text> <fontSize> <newWidth> <newHeight>" << endl;
        return 1;
    }

    string text = argv[1];
    int fontSize, newWidth, newHeight;

    try {
        fontSize = stoi(argv[2]);
        newWidth = stoi(argv[3]);
        newHeight = stoi(argv[4]);
    } catch (exception& e) {
        cerr << "Error: Invalid numeric input." << endl;
        return 1;
    }

    if (text.length() > 100) {
        cerr << "Error: Input text exceeds maximum length of 100 characters." << endl;
        return 1;
    }

    int width, height;
    BitmapImage original = renderTextToBitmap(text, fontSize, width, height);
    string originalFilename = "original_" + sanitizeFilename(text) + ".pgm";
    original.saveToFile(originalFilename);

    BitmapImage resized = resizeBitmap(original, newWidth, newHeight);
    string resizedFilename = "resized_" + sanitizeFilename(text) + ".pgm";
    resized.saveToFile(resizedFilename);

    cout << "Bitmap images saved as " << originalFilename << " and " << resizedFilename << endl;
    return 0;
}
