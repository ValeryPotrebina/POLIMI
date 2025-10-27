#include <Eigen/Dense>
#include <iostream>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;

MatrixXd convertToGrayscale(const MatrixXd& red, const MatrixXd& green, const MatrixXd& blue) {
    return 0.299 * red + 0.587 * green + 0.114 * blue;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <image_path>" << endl;
        return 1;
    }

    const char* image_path = argv[1];

    int width, height, channels;
    unsigned char* data = stbi_load(image_path, &width, &height, &channels, 0);
    if (!data) {
        cout << "Error: Failed to load image" << endl;
        return 1;
    }
    
    cout << "Image loaded successfully" << data.rows() << endl;
    cout << "Image loaded successfully" << data.cols() << endl;
    cout << "Image loaded successfully" << data.size() << endl;
    cout << "Image loaded successfully" << data.sum() << endl;
    cout << "Image loaded successfully" << data.mean() << endl;
    cout << "Image loaded successfully" << data.stdDev() << endl;
    cout << "Image loaded successfully" << data.minCoeff() << endl;
    cout << "Image loaded successfully" << data.maxCoeff() << endl;
    
}