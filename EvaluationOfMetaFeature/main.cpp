#include <opencv2\opencv.hpp>
#include <fstream>

#include "ConnectingDB\ConnectingDB.h"
#include "PatternDetector\PatternDetector.h"
#include "PathDetector.h"

bool readImages(std::vector<std::string> filenames, std::vector<cv::Mat>& images);

//コマンドライン引数にデータベースの名前を入力(*.accdb)
int main(int argc, char* argv[])
{
	if(argc < 1)
	{
		std::cout << "please input argumet" << std::cout;
		return 1;
	}
	//コマンドライン引数からDBの名前を取得
	std::string fileName = argv[1];


	System::String^ IMAGE_DIR = "C:\\Users\\satoshi\\Documents\\Image\\hiraizumi_kousen\\query";		// クエリ画像が保存されているフォルダ
	System::String^ DATABASE_IMG_DIR = "C:\\Users\\satoshi\\Documents\\Image\\hiraizumi_kousen\\query";	// DB用画像が保存されているフォルダ
	ConnectingDB db;
	PathDetector path;

	//結果出力ファイルの用意
	std::string txtName = "matchingResult_hiraizumi_kousen_";
	txtName += fileName;
	txtName += ".txt";
	std::ofstream txtFile(txtName);

	// フォルダの画像名を走査
	std::vector<std::string> backfilelist;	//画像の絶対パス
	std::vector<cv::Mat> queryImages;

	//クエリ画像の読み込み
	path.getPath(IMAGE_DIR, backfilelist);
	readImages(backfilelist, queryImages);

	//データベース構築に使った画像のパスを取得
	std::vector<std::string> databaseFilelist;	//画像の絶対パス
	path.getPath(DATABASE_IMG_DIR, databaseFilelist);

	//データベースから特徴量の読み込み
	std::vector<Pattern> patterns;
	db.loadDB(fileName, patterns);
	
	PatternDetector patternDetector(patterns);

	std::cout << "start" << std::endl;

	for(int i = 0; i < queryImages.size(); i++)
	{
		std::vector<int> ranking;
		patternDetector.findPattern(queryImages[i], ranking);
		
		std::vector<std::string> matchingList;
		matchingList.push_back(backfilelist[i]);
		matchingList.push_back(databaseFilelist[ranking[0]]);
		
		//結果をテキストファイルに出力
		txtFile << matchingList[0] << "	" << matchingList[1] << std::endl;
	}

	std::cout << "exit" << std::endl;

	return 0;
}

bool readImages(std::vector<std::string> filenames, std::vector<cv::Mat>& images)
{
	for(int i = 0; i < filenames.size(); i++)
	{

		cv::Mat image;

		image = cv::imread(filenames[i],1);			//画像の読み込み
		if (image.empty())
		{
			std::cout << "Input image cannot be read" << std::endl;
			return false;
		}
		//画像を追加
		images.push_back(image);

	}
}
