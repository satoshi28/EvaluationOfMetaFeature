#include <opencv2\opencv.hpp>
#include <fstream>

#include "ConnectingDB\ConnectingDB.h"
#include "PatternDetector\PatternDetector.h"
#include "PathDetector.h"

bool readImages(std::vector<std::string> filenames, std::vector<cv::Mat>& images);

//�R�}���h���C�������Ƀf�[�^�x�[�X�̖��O�����(*.accdb)
int main(int argc, char* argv[])
{
	if(argc < 1)
	{
		std::cout << "please input argumet" << std::cout;
		return 1;
	}
	//�R�}���h���C����������DB�̖��O���擾
	std::string fileName = argv[1];


	System::String^ IMAGE_DIR = "C:\\Users\\satoshi\\Documents\\Image\\hiraizumi_kousen\\query";		// �N�G���摜���ۑ�����Ă���t�H���_
	System::String^ DATABASE_IMG_DIR = "C:\\Users\\satoshi\\Documents\\Image\\hiraizumi_kousen\\query";	// DB�p�摜���ۑ�����Ă���t�H���_
	ConnectingDB db;
	PathDetector path;

	//���ʏo�̓t�@�C���̗p��
	std::string txtName = "matchingResult_hiraizumi_kousen_";
	txtName += fileName;
	txtName += ".txt";
	std::ofstream txtFile(txtName);

	// �t�H���_�̉摜���𑖍�
	std::vector<std::string> backfilelist;	//�摜�̐�΃p�X
	std::vector<cv::Mat> queryImages;

	//�N�G���摜�̓ǂݍ���
	path.getPath(IMAGE_DIR, backfilelist);
	readImages(backfilelist, queryImages);

	//�f�[�^�x�[�X�\�z�Ɏg�����摜�̃p�X���擾
	std::vector<std::string> databaseFilelist;	//�摜�̐�΃p�X
	path.getPath(DATABASE_IMG_DIR, databaseFilelist);

	//�f�[�^�x�[�X��������ʂ̓ǂݍ���
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
		
		//���ʂ��e�L�X�g�t�@�C���ɏo��
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

		image = cv::imread(filenames[i],1);			//�摜�̓ǂݍ���
		if (image.empty())
		{
			std::cout << "Input image cannot be read" << std::endl;
			return false;
		}
		//�摜��ǉ�
		images.push_back(image);

	}
}
