import sys
from PIL import Image

def image_to_text(input_image_path, output_file_path):
    try:
        img = Image.open(input_image_path)
        img = img.convert('RGB')

        width, height = img.size

        with open(output_file_path, 'w') as f:
            f.write(f"{width} {height}\n")
            
            for y in range(height):
                for x in range(width):
                    r, g, b = img.getpixel((x, y))
                    f.write(f"{r / 255:.3f} {g / 255:.3f} {b / 255:.3f}\n")

        print(f"Image successfully converted to {output_file_path}")
    
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 image_to_text.py <input_image> <output_file>")
        sys.exit(1)

    input_image = sys.argv[1]
    output_file = sys.argv[2]
    image_to_text(input_image, output_file)
