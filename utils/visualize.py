import numpy as np
import matplotlib.pyplot as plt
import sys

def read_image(filename):
    with open(filename, 'r') as f:
        width, height = map(int, f.readline().split())
        pixels = []
        for line in f:
            r, g, b = map(float, line.split())
            pixels.append([r, g, b])
        pixels = np.array(pixels).reshape((height, width, 3))
    return pixels

def display_image(image, title):
    plt.imshow(image)
    plt.title(title)
    plt.axis('off')
    plt.show()

# Main function to handle command-line argument and display image
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 display_image.py <image_file>")
        sys.exit(1)

    image_file = sys.argv[1]
    
    try:
        image = read_image(image_file)
        display_image(image, f"Displaying {image_file}")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
