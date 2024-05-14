#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

vector<unsigned char> readRawImage(const char* filename, int width, int height);
double computeMean(const vector<unsigned char>& image);
vector<double> zeroNormalizedCrossCorrelation(const vector<unsigned char>& input, const vector<unsigned char>& templateImage, int width, int height);
void saveImage(const char* filename, const vector<unsigned char>& image, int width, int height);

int main() 
{
    // Define image parameters
    // Change these parameters according to the image being tested
    const int width = 390;
    const int height = 355;

    // Load the input image and the template
    vector<unsigned char> inputImage = readRawImage("text_image.raw", width, height);
    vector<unsigned char> templateImage = readRawImage("letter_R.raw", 14, 10); // Adjust template dimensions

    // Perform zero-normalized cross-correlation
    vector<double> correlationResult = zeroNormalizedCrossCorrelation(inputImage, templateImage, width, height);

    // Normalize the cross-correlation result to the range [0, 255]
    double minVal = *min_element(correlationResult.begin(), correlationResult.end());
    double maxVal = *max_element(correlationResult.begin(), correlationResult.end());
    for (double& value : correlationResult) {
        value = 255.0 * (value - minVal) / (maxVal - minVal);
    }

    // Convert the result to an unsigned char grayscale image
    vector<unsigned char> outputImage(correlationResult.begin(), correlationResult.end());

    // Save the output image
    saveImage("output_image_zero_normalized.raw", outputImage, width, height);

    return 0;
}


// Function to read a raw image file
vector<unsigned char> readRawImage(const char* filename, int width, int height) {
    vector<unsigned char> image(width * height);
    ifstream file(filename, ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(image.data()), width * height);
        file.close();
    }
    return image;
}

// Function to compute the mean of an image
double computeMean(const vector<unsigned char>& image) {
    int sum = 0;
    for (unsigned char pixel : image) {
        sum += static_cast<int>(pixel);
    }
    return static_cast<double>(sum) / image.size();
}

// Function to perform zero-normalized cross-correlation
vector<double> zeroNormalizedCrossCorrelation(const vector<unsigned char>& input, const vector<unsigned char>& templateImage, int width, int height) {
    vector<double> output(width * height, 0.0);

    int templateWidth = templateImage.size() == width * height ? width : templateImage.size();
    int templateHeight = templateImage.size() == width * height ? height : templateImage.size();

    double meanInput = computeMean(input);
    double meanTemplate = computeMean(templateImage);

    for (int y = 0; y < height - templateHeight; ++y) {
        for (int x = 0; x < width - templateWidth; ++x) {
            double sum = 0.0;
            double sumInput = 0.0;
            double sumTemplate = 0.0;

            for (int ty = 0; ty < templateHeight; ++ty) {
                for (int tx = 0; tx < templateWidth; ++tx) {
                    int inputPixel = input[(y + ty) * width + (x + tx)];
                    int templatePixel = templateImage[ty * templateWidth + tx];
                    sum += (static_cast<double>(inputPixel) - meanInput) * (static_cast<double>(templatePixel) - meanTemplate);
                    sumInput += (static_cast<double>(inputPixel) - meanInput) * (static_cast<double>(inputPixel) - meanInput);
                    sumTemplate += (static_cast<double>(templatePixel) - meanTemplate) * (static_cast<double>(templatePixel) - meanTemplate);
                }
            }

            output[y * width + x] = sum / (sqrt(sumInput) * sqrt(sumTemplate));
        }
    }

    return output;
}

// Function to save the grayscale image
void saveImage(const char* filename, const vector<unsigned char>& image, int width, int height) {
    ofstream file(filename, ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(image.data()), width * height);
        file.close();
    }
}