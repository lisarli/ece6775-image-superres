#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream> 
#include <cmath>
#include <cstdlib>

using namespace std;

struct Pixel {
    float r, g, b;
};

vector<vector<Pixel>> convolve(const vector<vector<Pixel>>& input, const vector<vector<float>>& kernel) {
    int kernelSize = kernel.size();
    int height = input.size();
    int width = input[0].size();

    vector<vector<Pixel>> output(height, vector<Pixel>(width));

    for (int i = kernelSize / 2; i < height - kernelSize / 2; ++i) {
        for (int j = kernelSize / 2; j < width - kernelSize / 2; ++j) {
            Pixel result = {0.0f, 0.0f, 0.0f};
            for (int ki = -kernelSize / 2; ki <= kernelSize / 2; ++ki) {
                for (int kj = -kernelSize / 2; kj <= kernelSize / 2; ++kj) {
                    result.r += input[i + ki][j + kj].r * kernel[ki + kernelSize / 2][kj + kernelSize / 2];
                    result.g += input[i + ki][j + kj].g * kernel[ki + kernelSize / 2][kj + kernelSize / 2];
                    result.b += input[i + ki][j + kj].b * kernel[ki + kernelSize / 2][kj + kernelSize / 2];
                }
            }
            output[i][j] = result;
        }
    }
    return output;
}

Pixel relu(Pixel p) {
    return {max(0.0f, p.r), max(0.0f, p.g), max(0.0f, p.b)};
}

// upsample using nearest-neighbor interpolation
vector<vector<Pixel>> upsample(const vector<vector<Pixel>>& input, int scale) {
    int height = input.size();
    int width = input[0].size();
    int newHeight = height * scale;
    int newWidth = width * scale;

    vector<vector<Pixel>> output(newHeight, vector<Pixel>(newWidth));

    for (int i = 0; i < newHeight; ++i) {
        for (int j = 0; j < newWidth; ++j) {
            int srcI = i / scale;
            int srcJ = j / scale;
            output[i][j] = input[srcI][srcJ];
        }
    }
    return output;
}

// basic pipeline
vector<vector<Pixel>> superResolution(const vector<vector<Pixel>>& input, const vector<vector<float>>& kernel, int scale) {
    auto upscaledImage = upsample(input, scale);

    auto convolvedImage = convolve(upscaledImage, kernel);

    for (auto& row : convolvedImage) {
        for (auto& pixel : row) {
            pixel = relu(pixel);
        }
    }

    return convolvedImage;
}

// create basic kernel
vector<vector<float>> createKernel(int size) {
    return vector<vector<float>>(size, vector<float>(size, 1.0f / (size * size)));
}

// read in image
vector<vector<Pixel>> readImage(const string& filename, int& width, int& height) {
    ifstream file(filename);
    file >> width >> height;

    vector<vector<Pixel>> image(height, vector<Pixel>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file >> image[i][j].r >> image[i][j].g >> image[i][j].b;
        }
    }
    return image;
}

// write super-resolution image
void writeImage(const string& filename, const vector<vector<Pixel>>& image) {
    int height = image.size();
    int width = image[0].size();

    ofstream file(filename);
    file << width << " " << height << endl;

    for (const auto& row : image) {
        for (const auto& pixel : row) {
            file << fixed << setprecision(2) << pixel.r << " " << pixel.g << " " << pixel.b << endl;
        }
    }
}

// helper to print image
void printImage(const vector<vector<Pixel>>& image) {
    for (const auto& row : image) {
        for (const auto& pixel : row) {
            cout << fixed << setprecision(2)
                 << "[" << setw(5) << pixel.r << ", " << setw(5) << pixel.g << ", " << setw(5) << pixel.b << "] ";
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    int width, height;
    int scale = 2;
    string inputFile = "input_image.txt";
    string outputFile = "output_image.txt";

    auto inputImage = readImage(inputFile, width, height);
    auto kernel = createKernel(3);
    auto result = superResolution(inputImage, kernel, scale);
    writeImage(outputFile, result);

    cout << "Super-resolution image saved to " << outputFile << endl;

    return 0;
}