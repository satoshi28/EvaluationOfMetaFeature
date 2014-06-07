#ifndef MATCHING_
#define MATCHING_

////////////////////////////////////////////////////////////////////
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>

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
	int getMatches(const Pattern queryPattern);

	void train(const std::vector<Pattern> trainPatterns );
private:	
	//テンプレート画像からPatternを作成
	

	void match(std::vector<cv::KeyPoint> queryKeypoints,cv::Mat queryDescriptors,
				std::vector<cv::DMatch>& matches);

	//幾何学的整合性チェック
	bool geometricConsistencyCheck(std::vector<cv::KeyPoint> queryKeypoints, std::vector<cv::KeyPoint> trainKeypoints, std::vector<cv::DMatch>& match);
private:
    
	//画像セットの数
	int dataSetSize;
    cv::Ptr<cv::DescriptorMatcher> m_matcher;

};


#endif