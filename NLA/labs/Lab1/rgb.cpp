#include <Eigen/Dense>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

MatrixXd convertToGrayscale(const MatrixXd& red, const MatrixXd& green, const MatrixXd& blue) {
    return 0.299 * red + 0.587 * green + 0.114 * blue;
}