#include <opencv2\opencv.hpp>
#include <fstream>

#include "ConnectingDB\ConnectingDB.h"
#include "PatternDetector\PatternDetector.h"
#include "PathDetector.h"



bool readImages(std::vector<std::string> filenames, std::vector<cv::Mat>& images);
int getResult(std::vector<std::string> filelist, cv::Mat& resultImage);

int main()
{
	System::String^ IMAGE_DIR = "C:\\Users\\satoshi\\Documents\\Image\\ZuBuD\\query";	// 画像が保存されているフォルダ
	System::String^ DATABASE_IMG_DIR = "C:\\Users\\satoshi\\Documents\\Image\\ZuBuD\\databaseImage";	// 画像が保存されているフォルダ
	ConnectingDB db;
	PathDetector path;

	std::ofstream txtFile("matchingReslut.txt");

	std::vector<Pattern> patterns;

	// フォルダの画像名を走査
	std::vector<std::string> backfilelist;	//画像の絶対パス
	std::vector<cv::Mat> queryImages;

	path.getPath(IMAGE_DIR, backfilelist);
	readImages(backfilelist, queryImages);

	//データベース画像の読み込み
	std::vector<std::string> databaseFilelist;	//画像の絶対パス
	std::vector<cv::Mat> databaseImages;

	path.getPath(DATABASE_IMG_DIR, databaseFilelist);


	//データベースから特徴量の読み込み
	db.loadDB(-1, -1, patterns);
	
	PatternDetector patternDetector(patterns);

	std::cout << "start" << std::endl;

	int64 start = cv::getTickCount();

	for(int i = 0; i < queryImages.size(); i++)
	{
		std::vector<int> ranking;

		patternDetector.findPattern(queryImages[i], ranking);
		
		cv::Mat matchingResult;
		std::vector<std::string> matchingList;
		matchingList.push_back(backfilelist[i]);
		//3位まで結果を出す
		for(int i = 0; i < 3; i++)
			matchingList.push_back(databaseFilelist[ranking[i]]);
		
		//結果をテキストファイルに出力
		txtFile << matchingList[0] << "	" << matchingList[1] << std::endl;
		/*
		//結果を画像で出力
		getResult(matchingList, matchingResult);
		
		static int count = 0;
		std::stringstream ss;
		ss << count;
		std::string result = "result";
		result +=  ss.str();
		result += ".jpg";
		cv::imwrite(result,matchingResult);
		count++;
		*/
		
	}

	
	int64 end = cv::getTickCount();
	float time = (end - start) * 1000 / cv::getTickFrequency();

	time /= queryImages.size();
	std::cout << "average_time:"<< time << std::endl;
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

		image = cv::imread(a,1);			//画像の読み込み
		if (image.empty())
		{
			std::cout << "Input image cannot be read" << std::endl;
			return false;
		}
		//画像を追加
		images.push_back(image);

	}
}

int getResult(std::vector<std::string> filelist, cv::Mat& resultImage)
{
	int total_width =0;
	int height = 0;

	std::vector<cv::Mat> images;
	for(int i = 0; i < filelist.size(); i++)
	{

		cv::Mat image;
		image = cv::imread(filelist[i],1);			//画像の読み込み
		if (image.empty())
		{
			std::cout << "Input image cannot be read" << std::endl;
			return false;
		}
		//画像を追加
		images.push_back(image);

		total_width += image.cols;
		height = image.rows;
	}

	cv::Mat combinedImage(cv::Size(total_width, height), CV_8UC3);

	std::vector<cv::Mat>::iterator it  = images.begin(), it_end = images.end();
	cv::Rect roi_rect;
	int cnt = 0;
	for (; it != it_end; ++it) {
		roi_rect.width = it->cols;
		roi_rect.height = it->rows;
 
		cv::Mat roi(combinedImage, roi_rect);
		it->copyTo(roi);
		roi_rect.x += it->cols;
 
		cnt++;
	}
	resultImage = combinedImage;
}