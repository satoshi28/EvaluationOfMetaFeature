#ifndef MATCHING_
#define MATCHING_

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

    Matching(cv::Ptr<cv::DescriptorMatcher>   matcher   = cv::DescriptorMatcher::create(matcherName));

	void getMatches(const Pattern queryPattern, std::vector<int>& matchingList);

	void train(const std::vector<Pattern> trainPatterns );
private:	

	void match(std::vector<cv::KeyPoint> queryKeypoints,cv::Mat queryDescriptors,cv::Ptr<cv::DescriptorMatcher>& matcher,
				std::vector<cv::DMatch>& matches);
private:
    
	//‰æ‘œƒZƒbƒg‚Ì”
	int dataSetSize;
    cv::Ptr<cv::DescriptorMatcher> m_matcher;
	std::vector<Pattern> m_trainPatterns;
};


#endif