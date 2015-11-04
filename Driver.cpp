// Driver.cpp		Author: Sam Hoover
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;

const int SIZE = 4;
const int BUCKET_SIZE = 256 / SIZE;
const int DIMS = 3;
const int RED_PIX = 2;
const int GREEN_PIX = 1;
const int BLUE_PIX = 0;

// createColorHistorgram
// creates a 4x4x4 histogram of the colors in image. Each pixel's rgb value is
// assigned to a bucket by taking each rgb value divided by BUCKET_SIZE.
// preconditions:	none
// postconditions:	returns a 4x4x4 color histogram of image.
//
Mat createColorHistogram(const Mat& image);

// createOverlayImage
// creates a new image in which the most common color in foreground is replaced
// with a corresponding pixel from background. If foreground is larger than 
// background than background will repeat at foreground.row % background.row and
// foreground.col % background.col.
// preconditions:	foreground and background must be valid Mat objects storing
//					images
// postconditions:	returns a new Mat in which the most common color in foreground
//					is replaced with a corresponding pixel in background.
//
Mat createOverlayImage(const Mat& foreground, const Mat& background);

// findMostCommonColor
// finds the most common color in hist and sets r, g, b to values corresponding
// to this color. 
// preconditions:	hist must be a 3 dimensional Mat representing the color
//					histogram of an image
// postconditions:	sets r, g, b to the values corresponding to the most common
//					color in hist
//
void findMostCommonColor(int &r, int &g, int &b, const Mat& hist);

// replaceMostCommonColor
// replaces the most common color in target with a corresponding pixel from
// replacement.  If target is larger than replacement than replacement will
// repeat at target.row % replacement.row and target.col % replacement.col.
// preconditions:	r, g, b must represent the most common r, g, b values in 
//					target.
// postconditions:	replaces the most common color in target with a corresponding
//					pixel from replacement
//
void replaceMostCommonColor(int r, int g, int b, Mat& target, const Mat& replacement);

// main
// Reads two images, foreground.jpg and background.jpg and replaces the most 
// common color in foreground with a corresponding pixel from background. This
// new image is saved to disc as overlay.jpg. background.jpg is then flipped,
// converted to grayscale, smoothed, and has edges detected. This is saved to
// disc as output.jpg. Both overlay.jpg and output.jpg are then displayed to
// the screen.
// foreground.jpg and background.jpg must be located in the same file location
// as this file, Driver.cpp. overlay.jpg and output.jpg will be written to this
// same location.
//
int main(int argc, char* argv[]) {
	Mat foreground = imread("foreground.jpg");
	Mat background = imread("background.jpg");

	// create overlay.jpg
	Mat overlay = createOverlayImage(foreground, background);
	imwrite("overlay.jpg", overlay);

	// create output.jpg
	flip(background, background, 1);
	cvtColor(background, background, COLOR_BGR2GRAY);
	GaussianBlur(background, background, Size(7, 7), 2.0, 2.0);
	Canny(background, background, 20.0, 60.0);
	imwrite("output.jpg", background);

	//display overlay.jpg in window
	namedWindow("Overlay Image");
	imshow("Overlay Image", overlay);
	waitKey(0);

	//display output.jpg in window
	namedWindow("Output Image");
	imshow("Output Image", background);
	waitKey(0);

	return(0);
}

// createColorHistorgram
// creates a 4x4x4 histogram of the colors in image. Each pixel's rgb value is
// assigned to a bucket by taking each rgb value divided by BUCKET_SIZE.
// preconditions:	none
// postconditions:	returns a 4x4x4 color histogram of image.
//
Mat createColorHistogram(const Mat& image) {
	int dims[DIMS] = {SIZE, SIZE, SIZE};
	Mat hist(3, dims, CV_32S, Scalar::all(0));
	for(int row = 0; row < image.rows - 1; row++) {
		for(int col = 0; col < image.cols - 1; col++) {
			int r = static_cast<int>(image.at<Vec3b>(row, col)[RED_PIX] / BUCKET_SIZE);
			int g = static_cast<int>(image.at<Vec3b>(row, col)[GREEN_PIX] / BUCKET_SIZE);
			int b = static_cast<int>(image.at<Vec3b>(row, col)[BLUE_PIX] / BUCKET_SIZE);
			hist.at<int>(r, g, b)++;
		}
	}
	return(hist);
}


// createOverlayImage
// creates a new image in which the most common color in foreground is replaced
// with a corresponding pixel from background. If foreground is larger than 
// background than background will repeat at foreground.row % background.row and
// foreground.col % background.col.
// preconditions:	foreground and background must be valid Mat objects storing
//					images
// postconditions:	returns a new Mat in which the most common color in foreground
//					is replaced with a corresponding pixel in background.
//
Mat createOverlayImage(const Mat& foreground, const Mat& background) {
	int mostCommon_r = 0;
	int mostCommon_g = 0;
	int mostCommon_b = 0;
	Mat output = foreground.clone();
	Mat hist = createColorHistogram(foreground);
	findMostCommonColor(mostCommon_r, mostCommon_g, mostCommon_b, hist);
	replaceMostCommonColor(mostCommon_r, mostCommon_g, mostCommon_b, output, background);
	return(output);
}

// findMostCommonColor
// finds the most common color in hist and sets r, g, b to values corresponding
// to this color. 
// preconditions:	hist must be a 3 dimensional Mat representing the color
//					histogram of an image
// postconditions:	sets r, g, b to the values corresponding to the most common
//					color in hist
//
void findMostCommonColor(int &r, int &g, int &b, const Mat& hist) {
	int highestValue = hist.at<int>(0, 0, 0);
	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
			for(int k = 0; k < SIZE; k++) {
				if(hist.at<int>(i, j, k) > highestValue) {
					r = i;
					g = j;
					b = k;
					highestValue = hist.at<int>(i, j, k);
				}
			}
		}
	}

	r = static_cast<int>(r * BUCKET_SIZE + BUCKET_SIZE / 2);
	g = static_cast<int>(g * BUCKET_SIZE + BUCKET_SIZE / 2);
	b = static_cast<int>(b * BUCKET_SIZE + BUCKET_SIZE / 2);
}

// replaceMostCommonColor
// replaces the most common color in target with a corresponding pixel from
// replacement.  If target is larger than replacement than replacement will
// repeat at target.row % replacement.row and target.col % replacement.col.
// preconditions:	r, g, b must represent the most common r, g, b values in 
//					target.
// postconditions:	replaces the most common color in target with a corresponding
//					pixel from replacement
//
void replaceMostCommonColor(int r, int g, int b, Mat& target, const Mat& replacement) {
	for(int i = 0; i < target.rows - 1; i++) {
		for(int j = 0; j < target.cols - 1; j++) {
			if(target.at<Vec3b>(i, j)[RED_PIX] - BUCKET_SIZE <= r &&
			   target.at<Vec3b>(i, j)[RED_PIX] + BUCKET_SIZE >= r &&
			   target.at<Vec3b>(i, j)[GREEN_PIX] - BUCKET_SIZE <= g &&
			   target.at<Vec3b>(i, j)[GREEN_PIX] + BUCKET_SIZE >= g &&
			   target.at<Vec3b>(i, j)[BLUE_PIX] - BUCKET_SIZE <= b &&
			   target.at<Vec3b>(i, j)[BLUE_PIX] + BUCKET_SIZE >= b) {
				target.at<Vec3b>(i, j)[RED_PIX] = replacement.at<Vec3b>(i % replacement.rows, j % replacement.cols)[RED_PIX];
				target.at<Vec3b>(i, j)[GREEN_PIX] = replacement.at<Vec3b>(i % replacement.rows, j % replacement.cols)[GREEN_PIX];
				target.at<Vec3b>(i, j)[BLUE_PIX] = replacement.at<Vec3b>(i % replacement.rows, j % replacement.cols)[BLUE_PIX];
			}
		}
	}
}

