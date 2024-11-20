// superress.cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "weights.h" // Include the weights and biases

using namespace std;

// Define the ReLU activation function
inline float ReLU(float x) {
    return x > 0 ? x : 0;
}

// Function to read a grayscale image from a text file
vector<vector<float>> readImage(const string& filename, int& width, int& height) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening input file: " << filename << endl;
        exit(1);
    }

    file >> width >> height;

    // Validate dimensions
    if (width <= 0 || height <= 0) {
        cerr << "Invalid image dimensions: " << width << "x" << height << endl;
        exit(1);
    }

    vector<vector<float>> image(height, vector<float>(width, 0.0f));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (!(file >> image[i][j])) {
                cerr << "Error reading pixel data at (" << i << ", " << j << ")" << endl;
                exit(1);
            }
            // Normalize pixel values if necessary
            // image[i][j] /= 255.0f; // Uncomment if input pixel values are in the range [0, 255]
        }
    }

    file.close();
    cout << "Successfully read input image: " << width << "x" << height << endl;
    return image;
}

// Function to write a grayscale image to a text file
void writeImage(const string& filename, const vector<vector<float>>& image) {
    int height = image.size();
    int width = image[0].size();

    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        exit(1);
    }

    file << width << " " << height << endl;

    for (const auto& row : image) {
        for (const auto& pixel : row) {
            file << fixed << setprecision(6) << pixel << endl;
        }
    }

    file.close();
    cout << "Successfully wrote output image: " << width << "x" << height << endl;
}

// Function to perform convolution with 'same' padding and 'replicate' edge handling
void convolve2D(const vector<vector<float>>& input, vector<vector<float>>& output,
                const vector<vector<float>>& kernel, float bias, bool applyReLU) {
    int height = input.size();
    int width = input[0].size();
    int ksize = kernel.size();
    int pad = ksize / 2;

    output.resize(height, vector<float>(width, 0.0f));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float sum = 0.0f;
            for (int m = -pad; m <= pad; m++) {
                for (int n = -pad; n <= pad; n++) {
                    int ii = min(max(i + m, 0), height - 1);
                    int jj = min(max(j + n, 0), width - 1);
                    sum += input[ii][jj] * kernel[m + pad][n + pad];
                }
            }
            sum += bias;
            if (applyReLU) {
                sum = ReLU(sum);
            }
            output[i][j] = sum;
        }
    }
}

// Upsample using nearest-neighbor interpolation
vector<vector<float>> upsample(const vector<vector<float>>& input, int scale) {
    int height = input.size();
    int width = input[0].size();
    int newHeight = height * scale;
    int newWidth = width * scale;

    vector<vector<float>> output(newHeight, vector<float>(newWidth, 0.0f));

    for (int i = 0; i < newHeight; ++i) {
        int srcI = i / scale;
        for (int j = 0; j < newWidth; ++j) {
            int srcJ = j / scale;
            output[i][j] = input[srcI][srcJ];
        }
    }

    return output;
}

// Function to perform SRCNN
vector<vector<float>> SRCNN(const vector<vector<float>>& inputImage) {
    int height = inputImage.size();
    int width = inputImage[0].size();

    // Upsample the image using nearest-neighbor interpolation
    int scale = 2; // Assuming a scale factor of 2
    vector<vector<float>> im_b = upsample(inputImage, scale);
    int newHeight = im_b.size();
    int newWidth = im_b[0].size();

    cout << "Upscaled Image Size: " << newWidth << "x" << newHeight << endl;

    // Allocate memory for intermediate data
    int conv1_filters = 64;
    int conv1_patchsize = 9;
    vector<vector<vector<float>>> conv1_data(conv1_filters, vector<vector<float>>(newHeight, vector<float>(newWidth, 0.0f)));

    int conv2_filters = 32;
    int conv2_channels = conv1_filters;
    int conv2_patchsize = 5;
    vector<vector<vector<float>>> conv2_data(conv2_filters, vector<vector<float>>(newHeight, vector<float>(newWidth, 0.0f)));

    int conv3_channels = conv2_filters;
    int conv3_patchsize = 5;
    vector<vector<float>> im_h(newHeight, vector<float>(newWidth, 0.0f));

    // Conv1
    for (int f = 0; f < conv1_filters; f++) {
        // Reshape weights_conv1_reshaped[f] to 2D kernel
        vector<vector<float>> kernel(conv1_patchsize, vector<float>(conv1_patchsize, 0.0f));
        for (int m = 0; m < conv1_patchsize; m++) {
            for (int n = 0; n < conv1_patchsize; n++) {
                kernel[m][n] = weights_conv1_reshaped[f][m][n];
            }
        }
        // Perform convolution
        convolve2D(im_b, conv1_data[f], kernel, biases_conv1[f], true);
    }

    cout << "Conv1 Output: " << conv1_filters << " filters, " << newHeight << "x" << newWidth << endl;

    // Conv2
    for (int f = 0; f < conv2_filters; f++) {
        // Initialize sumData to zeros
        vector<vector<float>> sumData(newHeight, vector<float>(newWidth, 0.0f));
        for (int c = 0; c < conv2_channels; c++) {
            // Reshape conv2_subfilters[f][c] to 5x5 kernel
            // Reshape conv2_subfilters[f][c][25] to 5x5 kernel
                vector<vector<float>> kernel(conv2_patchsize, vector<float>(conv2_patchsize, 0.0f));
                for (int k = 0; k < 25; k++) {
                    int m = k / conv2_patchsize;
                    int n = k % conv2_patchsize;
                    kernel[m][n] = conv2_subfilters[f][c][k];
                }

            vector<vector<float>> convResult(newHeight, vector<float>(newWidth, 0.0f));
            convolve2D(conv1_data[c], convResult, kernel, 0.0f, false);
            // Accumulate the result
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    sumData[i][j] += convResult[i][j];
                }
            }
        }
        // Add bias and apply ReLU
        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                sumData[i][j] += biases_conv2[f];
                sumData[i][j] = ReLU(sumData[i][j]);
                conv2_data[f][i][j] = sumData[i][j];
            }
        }
    }

    cout << "Conv2 Output: " << conv2_filters << " filters, " << newHeight << "x" << newWidth << endl;

    // Conv3
    // Initialize im_h to zeros
    vector<vector<float>> sumData(newHeight, vector<float>(newWidth, 0.0f));
    for (int c = 0; c < conv3_channels; c++) {
        // Reshape conv3_subfilters[c] to 5x5 kernel
        vector<vector<float>> kernel(conv3_patchsize, vector<float>(conv3_patchsize, 0.0f));
        for (int m = 0; m < conv3_patchsize; m++) {
            for (int n = 0; n < conv3_patchsize; n++) {
                kernel[m][n] = conv3_subfilters[c][m][n];
            }
        }
        vector<vector<float>> convResult(newHeight, vector<float>(newWidth, 0.0f));
        convolve2D(conv2_data[c], convResult, kernel, 0.0f, false);
        // Accumulate the result
        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                sumData[i][j] += convResult[i][j];
            }
        }
    }
    // Add bias
    for (int i = 0; i < newHeight; i++) {
        for (int j = 0; j < newWidth; j++) {
            im_h[i][j] = sumData[i][j] + biases_conv3[0];
            // Clip the pixel values to [0.0, 1.0]
            im_h[i][j] = min(max(im_h[i][j], 0.0f), 1.0f);
        }
    }

    cout << "Conv3 Output: " << newHeight << "x" << newWidth << endl;

    return im_h;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_image.txt> <output_image.txt>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    int width, height;

    // Read the input image (grayscale)
    vector<vector<float>> inputImage = readImage(inputFile, width, height);

    // Perform SRCNN
    vector<vector<float>> highResImage = SRCNN(inputImage);

    // Write the high-resolution image
    writeImage(outputFile, highResImage);

    cout << "Super-resolution image saved to " << outputFile << endl;

    return 0;
}
