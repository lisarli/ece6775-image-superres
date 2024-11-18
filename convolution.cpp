// Function to apply 'same' padding with 'replicate' edge handling
std::vector<std::vector<float>> padImage(const std::vector<std::vector<float>>& image, int pad) {
    int original_height = image.size();
    int original_width = image[0].size();
    int padded_height = original_height + 2 * pad;
    int padded_width = original_width + 2 * pad;
    std::vector<std::vector<float>> padded_image(padded_height, std::vector<float>(padded_width, 0.0f));

    for(int i = 0; i < padded_height; ++i) {
        for(int j = 0; j < padded_width; ++j) {
            int orig_i = std::min(std::max(i - pad, 0), original_height - 1);
            int orig_j = std::min(std::max(j - pad, 0), original_width - 1);
            padded_image[i][j] = image[orig_i][orig_j];
        }
    }
    return padded_image;
}

// Function to perform convolution
std::vector<std::vector<float>> convolve(const std::vector<std::vector<float>>& input,
                                        const std::vector<std::vector<float>>& kernel,
                                        float bias,
                                        bool applyReLU) {
    int kernel_size = kernel.size();
    int pad = kernel_size / 2;
    int height = input.size();
    int width = input[0].size();

    // Pad the input image
    std::vector<std::vector<float>> padded_input = padImage(input, pad);

    // Initialize output image
    std::vector<std::vector<float>> output(height, std::vector<float>(width, 0.0f));

    // Perform convolution
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            float sum = 0.0f;
            for(int ki = 0; ki < kernel_size; ki++) {
                for(int kj = 0; kj < kernel_size; kj++) {
                    sum += padded_input[i + ki][j + kj] * kernel[ki][kj];
                }
            }
            sum += bias;
            if(applyReLU) {
                sum = std::max(0.0f, sum);
            }
            output[i][j] = sum;
        }
    }

    return output;
}
