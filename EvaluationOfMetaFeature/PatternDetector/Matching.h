#ifndef MATCHING_
#define MATCHING_

////////////////////////////////////////////////////////////////////
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <functional>

#include "../Pattern.h"
#include "../CONSTANT.h"


/**
 * Store the image data and computed descriptors of target pattern
 */
class Matching
{
public:
	/**
     *
     */
    Matching();


    /**
    * 
    */
	void getMatches(const Pattern queryPattern, std::vector<int>& matchingList);

	void train(const std::vector<Pattern> trainPatterns );
private:	
	//�e���v���[�g�摜����Pattern���쐬
	

	void match(cv::Mat queryDescriptors,cv::Mat dbDescriptors, std::vector<cv::DMatch>& matches);

	//�􉽊w�I�������`�F�b�N
	bool geometricConsistencyCheck(std::vector<cv::KeyPoint> queryKeypoints, std::vector<cv::KeyPoint> trainKeypoints, std::vector<cv::DMatch>& match);
private:
    
	//�摜�Z�b�g�̐�
	int dataSetSize;
    std::vector< cv::Ptr<cv::DescriptorMatcher>> m_matchers;
	std::vector<Pattern> m_trainPatterns;
};


#endif