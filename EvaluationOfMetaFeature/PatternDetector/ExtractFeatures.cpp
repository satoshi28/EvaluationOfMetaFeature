#include "ExtractFeatures.h"

ExtractFeatures::ExtractFeatures(cv::SurfFeatureDetector detector, 
    cv::Ptr<cv::DescriptorExtractor> extractor
	)
    : m_detector(detector)
    , m_extractor(extractor)
{
	
}

ExtractFeatures::~ExtractFeatures()
{
}

bool ExtractFeatures::getFeatures(cv::Mat& image,
									  Pattern& pattern)
{
	bool extractFlag = false;

	//グレイスケール化
	cv::Mat grayImg;										
	getGray(image, grayImg);

	//特徴量の抽出
	extractFlag = extractFeatures(grayImg, pattern.keypoints, pattern.descriptors);
	if(extractFlag == false)
		return extractFlag;

	pattern.image = image.clone();

	return extractFlag;
}

void ExtractFeatures::getGray(const cv::Mat& image, cv::Mat& gray)
{
    if (image.channels()  == 3)
        cv::cvtColor(image, gray, CV_BGR2GRAY);
    else if (image.channels() == 4)
        cv::cvtColor(image, gray, CV_BGRA2GRAY);
    else if (image.channels() == 1)
        gray = image;
}

bool ExtractFeatures::extractFeatures(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors)
{
    assert(!image.empty());
    assert(image.channels() == 1);

    m_detector.detect(image, keypoints);
    if (keypoints.empty())
        return false;

    m_extractor->compute(image, keypoints, descriptors);
    if (keypoints.empty())
        return false;

    return true;
}