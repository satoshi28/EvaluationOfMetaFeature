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
	//テンプレート画像からPatternを作成
	

	void match(cv::Mat queryDescriptors,cv::Mat dbDescriptors, std::vector<cv::DMatch>& matches);

	//幾何学的整合性チェック
	bool geometricConsistencyCheck(std::vector<cv::KeyPoint> queryKeypoints, std::vector<cv::KeyPoint> trainKeypoints, std::vector<cv::DMatch>& match);
private:
    
	//画像セットの数
	int dataSetSize;
    std::vector< cv::Ptr<cv::DescriptorMatcher>> m_matchers;
	std::vector<Pattern> m_trainPatterns;
};


#endif