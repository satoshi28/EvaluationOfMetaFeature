#include <opencv2\opencv.hpp>

#include "ConnectingDB\ConnectingDB.h"
#include "PatternDetector\PatternDetector.h"
int main()
{
	ConnectingDB db;
	
	std::vector<Pattern> patterns;

	db.loadDB(38.0, 141.0, patterns);
	
	cv::Mat image = cv::imread("C:\\Users\\satoshi\\Documents\\Image\\gate\\Gate.view02.JPG",1);

	PatternDetector patternDetector(patterns);
	int matchedNumber = patternDetector.findPattern(image);

	std::cout << matchedNumber << std::endl;
	//cv::imshow("matchingResult", patterns[matchedNumber].image);

	cv::waitKey(0);
	return 0;
}