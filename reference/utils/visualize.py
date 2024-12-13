import numpy as np
import matplotlib.pyplot as plt
import sys
import math

def read_image(filename):
    with open(filename, 'r') as f:
        first_line = f.readline().strip()
        try:
            # try parsing width and height
            width, height = map(int, first_line.split())
        except ValueError:
            # parse as all pixels
            f.seek(0)
            lines = f.readlines()
            num_pixels = len(lines)
            width = height = int(math.sqrt(num_pixels))
            if width * height != num_pixels:
                raise ValueError("The number of lines in the file does not form a perfect square.")
            pixels = [list(map(float, line.split())) for line in lines]
        else:
            pixels = [list(map(float, line.split())) for line in f]
        
        pixels = np.array(pixels).reshape((height, width, 3))
    return pixels

def display_images(image1, image2, title1, title2):
    plt.figure(figsize=(10, 5))
    
    # display blurry image
    plt.subplot(1, 2, 1)
    plt.imshow(image1)
    plt.title(title1)
    plt.axis('off')
    
    # display sharpened image
    plt.subplot(1, 2, 2)
    plt.imshow(image2)
    plt.title(title2)
    plt.axis('off')
    
    plt.tight_layout()
    plt.show()

# Main function to handle command-line arguments and display images
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 display_images.py <blurry_image_file> <sharpened_image_file>")
        sys.exit(1)

    blurry_image_file = sys.argv[1]
    sharpened_image_file = sys.argv[2]
    
    try:
        blurry_image = read_image(blurry_image_file)
        sharpened_image = read_image(sharpened_image_file)
        display_images(blurry_image, sharpened_image, "Blurry Image", "Sharpened Image")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
