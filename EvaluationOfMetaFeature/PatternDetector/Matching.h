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
    Matching(cv::Ptr<cv::DescriptorMatcher>   matcher   = cv::DescriptorMatcher::create(matcherName));


    /**
    * 
    */
	void getMatches(const Pattern queryPattern, std::vector<int>& matchingList);

	void train(const std::vector<Pattern> trainPatterns );
private:	
	//�e���v���[�g�摜����Pattern���쐬
	

	void match(std::vector<cv::KeyPoint> queryKeypoints,cv::Mat queryDescriptors,cv::Ptr<cv::DescriptorMatcher>& matcher,
				std::vector<cv::DMatch>& matches);

	//�􉽊w�I�������`�F�b�N
	bool geometricConsistencyCheck(std::vector<cv::KeyPoint> queryKeypoints, std::vector<cv::KeyPoint> trainKeypoints, std::vector<cv::DMatch>& match);
private:
    
	//�摜�Z�b�g�̐�
	int dataSetSize;
    cv::Ptr<cv::DescriptorMatcher> m_matcher;
	std::vector<Pattern> m_trainPatterns;
	cv::FlannBasedMatcher LSHmatcher;
};


#endif