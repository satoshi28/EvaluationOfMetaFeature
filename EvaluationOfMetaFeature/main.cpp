#include <opencv2\opencv.hpp>
#include <fstream>

#include "ConnectingDB\ConnectingDB.h"
#include "PatternDetector\PatternDetector.h"
#include "PathDetector.h"



bool readImages(std::vector<std::string> filenames, std::vector<cv::Mat>& images);

int main()
{
	System::String^ IMAGE_DIR = "C:\\Users\\satoshi\\Documents\\Image\\qimage\\";	// �摜���ۑ�����Ă���t�H���_

	ConnectingDB db;
	PathDetector path;

	std::ofstream txtFile("matchingReslut.txt");

	std::vector<Pattern> patterns;

	// �t�H���_�̉摜���𑖍�
	std::vector<std::string> backfilelist;	//�摜�̐�΃p�X
	std::vector<cv::Mat> queryImages;

	path.getPath(IMAGE_DIR, backfilelist);

	readImages(backfilelist, queryImages);

	db.loadDB(-1, -1, patterns);
	
	PatternDetector patternDetector(patterns);

	std::cout << "start" << std::endl;

	for(int i = 0; i < queryImages.size(); i++)
	{
		int matchedNumber = patternDetector.findPattern(queryImages[i]);

		txtFile << i+1 << "	" << matchedNumber+2 << std::endl;
	}

	std::cout << "stop" << std::endl;
	//cv::imshow("matchingResult", patterns[matchedNumber].image);

	cv::waitKey(0);
	return 0;
}

bool readImages(std::vector<std::string> filenames, std::vector<cv::Mat>& images)
{
	for(int i = 0; i < filenames.size(); i++)
	{

		cv::Mat image;
		std::string a = filenames[i];
		
		std::cout << a << std::endl;

		image = cv::imread(a,1);			//�摜�̓ǂݍ���
		if (image.empty())
		{
			std::cout << "Input image cannot be read" << std::endl;
			return false;
		}
		//�摜��ǉ�
		images.push_back(image);

	}
}