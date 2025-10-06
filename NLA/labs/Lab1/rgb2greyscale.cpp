#include <Eigen/Dense>
#include <iostream>

// from https://github.com/nothings/stb/tree/master
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace Eigen;

// Function to convert RGB to grayscale
MatrixXd convertToGrayscale(const MatrixXd& red, const MatrixXd& green,
                            const MatrixXd& blue) {
  return 0.299 * red + 0.587 * green + 0.114 * blue;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
    return 1;
  }

  const char* input_image_path = argv[1];

  // Load the image using stb_image
  int width, height, channels;
  unsigned char* image_data = stbi_load(input_image_path, &width, &height,
                                        &channels, 3);  // Force load as RGB
  if (!image_data) {
    std::cerr << "Error: Could not load image " << input_image_path
              << std::endl;
    return 1;
  }

  std::cout << "Image loaded: " << width << "x" << height << " with "
            << channels << " channels." << std::endl;

  // Prepare Eigen matrices for each RGB channel
  MatrixXd red(height, width), green(height, width), blue(height, width);

  // Fill the matrices with image data
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int index = (i * width + j) * 3;  // 3 channels (RGB)
      red(i, j) = static_cast<double>(image_data[index]) / 255.0;
      green(i, j) = static_cast<double>(image_data[index + 1]) / 255.0;
      blue(i, j) = static_cast<double>(image_data[index + 2]) / 255.0;
    }
  }
  // Free memory!!!
  stbi_image_free(image_data);

  // Create a grayscale matrix
  MatrixXd gray = convertToGrayscale(red, green, blue);

  Matrix<unsigned char, Dynamic, Dynamic, RowMajor> grayscale_image(height, width);
  // Use Eigen's unaryExpr to map the grayscale values (0.0 to 1.0) to 0 to 255
  grayscale_image = gray.unaryExpr([](double val) -> unsigned char {
    return static_cast<unsigned char>(val * 255.0);
  });

  // Save the grayscale image using stb_image_write
  const std::string output_image_path = "output_grayscale.png";
  if (stbi_write_png(output_image_path.c_str(), width, height, 1,
                     grayscale_image.data(), width) == 0) {
    std::cerr << "Error: Could not save grayscale image" << std::endl;
    
    return 1;
  }

  std::cout << "Grayscale image saved to " << output_image_path << std::endl;

  return 0;
}
